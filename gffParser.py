import numpy as np
#import modin.pandas as pd # not working 
import pandas as pd


warning_color = "\033[93m"
error_color = "\033[91m"
output_color = "\033[92m"
color_end = "\033[0m"


class GffParser:
    def __init__(self,filename):
        self.commonHeader = ['Seqid','Source','Feature','Start','Stop','Score','Strand','Phase']
        self.filename = filename
        self.commentMode = self.checkMode()
        print(f"Remove comment mode : {self.commentMode} file" )
        self.metadata,commentedData = self.extractMetaData()
        self.data = pd.read_csv(self.filename,sep="\t",header=None,skiprows=commentedData)
        print(f"{output_color}Data read into memory.{color_end}")
        self.feature_tags = set(self.data[2].unique())
        self.feature_objects = self.getFeatureTags()

    #Check the presence of comment lines. Return 'top' if comment lines are preent at the start of the file else return 'full'
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
        except IOError:
            print(f"{error_color}Error: File do not exist.")
            exit(0)

    #Seprate metadata into seprate objects and returns index of lines to skip
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
            print(f"{waring_color}Warning: Try using 'full' mode to read gff without comment lines.{color_end}")

    #Show all features in a gff file
    def showFeatureTags(self):
        print(f"Unique Features in GFF : {self.filename}")
        for i in self.feature_tags: 
            print(f"  {i}")

    #Show tags in info column given single feature as string or multiple feature as list of string
    def showgffFeatures(self,feature):
        typ = type(feature) 
        if typ == str:
            print("Common tags in feature :")
            print("  ",feature," : ",self.feature_objects[feature])
        elif typ == list:
            print("Common tags per feature :")
            for x in feature:
                print(f"\t{x} : {self.feature_objects[x]}")

    #Get Common tags per features in gff file
    def getFeatureTags(self):
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

    #Feature tags intersection 
    def getCommonTagsAmongFeatures(self,features):
        try:
            if type(features) == list:
                print()
        except:
            print()

    #Extract data with a feature
    def getDataWithSingleFeature(self,feature):
        try:
            if type(feature) != str:
                print(f"{error_color} ERROR : getDataWithFeature() gets data for single feature. Use getDataWithMultipleFeatures() for multiple feature.{color_end}")
                return
            else:
                header = self.commonHeader+list(self.feature_objects[feature])
                feature_tags = list(self.feature_objects[feature])
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
        except:
            if type(feature) == str:
                print(f"{error_color}ERROR : Feature \"{feature}\" do not exist in the file {self.filename} {color_end}")

    #Extract data with a set of features
    def getDataWithMultipleFeatures(self,features):
        try:
            if type(features) != list:
                print(f"{error_color}ERROR : getDataWithMultipleFeatures() gets data for multiple feature, use getDataWithFeature() to get data for single feature..{color_end}")
            else:
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
        except:
            print("some random error in getdatawithmultiple feature")

    #Write data to file
    def dataToFile(self,data,filename):
        try:
            data.to_csv(filename,sep="\t",index=False)
            print(f"{output_color}Data written to file: {filename}{color_end}")
        except:
            print(f"{error_color}ERROR : no file name provided")

    #Remove columns from data
    def removeColumn(self,data,columnName):
        try:
            if type(columnName) == list:
                for c_n in columnName:
                    if c_n not in data.columns:
                        print(f"{error_color}Error: Column name {c_n} do not exist in the selected data{color_end}")
                    else:
                        data.drop(columns=c_n)
                        print(f"{c_n} removed: done")
                return data
            elif type(columnName) == str:
                if columnName not in data.columns:
                    print(f"{error_color}Error: Column name {columnName} do not exist in the selected data{color_end}")
                else:
                    data.drop(columns=columnName)
                    print(f"{columnName} removed: done")
                return data
        except:
            print("exception")


    #def annotateBedCordinates(self,bedData):




filename = "sample.gff"
gffp = GffParser(filename)
gffp.checkMode()
# gffp.showFeatureTags()
# print(gffp.getFeatureTags())
# gffp.showgffFeatures(['CDS','mRNA'])
# data=gffp.getDataWithSingleFeature('mRNA')
data=gffp.getDataWithMultipleFeatures(['mRNA','CDS'])
# print(gffp.removeColumn(data,'Parent'))
# gffp.dataToFile(gffp.getDataWithSingleFeature('mRNA'),"output2.tsv")
gffp.dataToFile(gffp.getDataWithMultipleFeatures(['exon','CDS']),"output.tsv")
