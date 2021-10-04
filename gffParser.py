import numpy as np
import pandas as pd 

class GffParser:
    def __init__(self,filename):
        self.commonHeader = ['Seqid','Source','Feature','Start','Stop','Score','Strand','Phase']
        self.filename = filename
        self.commentMode = self.checkMode()
        print("Remove comment mode : %s file" %self.commentMode)
        self.metadata,commentedData = self.extractMetaData()
        self.data = pd.read_csv(self.filename,sep="\t",header=None,skiprows=commentedData)
        self.feature_tags = set(self.data[2].unique())
        self.feature_objects = self.getFeatureCommonTags()

    def checkMode(self):
        try:
            nextOcc = False
            fh = open(self.filename,'r')
            for lines in fh:
                if lines[0] == '#' and nextOcc:
                    return 'full'
                elif lines[0] != '#':
                    nextOcc = True
                else:
                    continue
            return 'top'
        except:
            print("File error")

    def extractMetaData(self):
        metadata = []
        skipLines = []
        try:
            if self.commentMode == 'top':
                i = -1
                fh = open(self.filename,'r')
                for lines in fh:
                    i += 1
                    if lines[0] == '#':
                        skipLines.append(i)
                        metadata.append(lines)
                    else:
                        fh.close()
                        return metadata,skipLines
            elif self.commentMode == 'full':
                i = -1
                fh = open(self.filename,'r')
                for lines in fh:
                    i += 1
                    if lines[0] == '#':
                        skipLines.append(i)
                        metadata.append(lines)
                    else:
                        continue
                fh.close()
                return metadata,skipLines
        except:
            print("Try using other 'full' mode to read gff without comment lines.")

    def showFeatureTags(self):
        print("Unique Features in GFF : ",self.filename)
        for i in self.feature_tags: 
            print("  ",i)

    def showgffFeatures(self,feature):
        print("Common tags in feature :")
        print("  ",feature," : ",self.feature_objects[feature])

    def getFeatureCommonTags(self):
        feature_common_tags = {}
        for features in self.feature_tags:
            feature_data = self.data[self.data[2] == features]
            totalFeatureRows = len(feature_data)
            info_tag_dict = {}
            for idx,fd in feature_data[8].items():
                info_col = fd.split(';')
                for info_i in info_col:
                    i_c = info_i.split('=')
                    if i_c[0] not in info_tag_dict.keys():
                        info_tag_dict[i_c[0]] = 1
                    else:
                        info_tag_dict[i_c[0]] = info_tag_dict[i_c[0]]+1
            common_tags = []
            for tag_counts in info_tag_dict.keys():
                if info_tag_dict[tag_counts] == totalFeatureRows:
                    common_tags.append(tag_counts)
            feature_common_tags[features] = set(common_tags)
        return feature_common_tags

    def getDataWithFeature(self,feature):
        header = self.commonHeader+self.feature_objects[feature]
        feature_tags = self.feature_objects[feature]
        feature_data = self.data[self.data[2] == feature]
        feature_info = []
        for i in range(0,len(feature_tags)):
            x = []
            feature_info.append(x)
        for idx,fd in feature_data[8].items():
            info_col = fd.split(';')
            for info_i in info_col:
                i_c = info_i.split('=')
                for i_f in range(0,len(feature_tags)):
                    if i_c[0] == feature_tags[i_f]:
                        feature_info[i_f].append(i_c[1])
        temp_data = feature_data.loc[:,[0,1,2,3,4,5,6,7]]
        temp_data.columns = self.commonHeader
        for tag_i in range(0,len(feature_tags)):
            temp_data[feature_tags[tag_i]] = feature_info[tag_i]
        return temp_data

    def getDataWithMultipleFeatures(self,features):
        common_subtags = {}
        for t in range(0,len(features)):
            if t == 0:
                common_subtags = self.feature_objects[features[t]].intersection(self.feature_objects[features[t]])
            else:
                common_subtags = common_subtags.intersection(self.feature_objects[features[t]])
        common_subtags = list(common_subtags)
        header = self.commonHeader+common_subtags
        mainTable = pd.DataFrame()
        for feature in features:
            feature_data = self.data[self.data[2] == feature]
            feature_info = []
            for i in range(0,len(common_subtags)):
                x = []
                feature_info.append(x)
            for idx,fd in feature_data[8].items():
                info_col = fd.split(';')
                for info_i in info_col:
                    i_c = info_i.split('=')
                    for i_f in range(0,len(common_subtags)):
                        if i_c[0] == common_subtags[i_f]:
                            feature_info[i_f].append(i_c[1])
            temp_data = feature_data.loc[:,[0,1,2,3,4,5,6,7]]
            temp_data.columns = self.commonHeader
            for tag_i in range(0,len(common_subtags)):
                temp_data[common_subtags[tag_i]] = feature_info[tag_i]
            mainTable = mainTable.append(temp_data)
        return mainTable

    def dataToFile(self,data,filename):
        data.to_csv(filename,sep="\t",index=False)


filename = "PM.genescf.gff"
gffp = GffParser(filename)
gffp.checkMode()
gffp.showFeatureTags()
# gffp.showgffFeatures('CDS')
# print(gffp.getDataWithFeature('CDS'))
# gffp.dataToFile(gffp.getDataWithFeature('CDS'),"output.tsv")
# gffp.dataToFile(gffp.getDataWithMultipleFeatures(['exon','CDS']),"output.tsv")
