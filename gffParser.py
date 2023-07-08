import numpy as np
import pandas as pd


class tabFileReader:
    def __init__(self,filename,bioFileType,metainfo_sep,val_sep):
        self.filename = filename
        self.fileType = bioFileType
        self.fileobj = open(filename,'r')
        self.colSep = metainfo_sep
        self.valSep = val_sep

        # set header for data based on file type
        self.fileHeader()
                
        # parse data from tab seperated file
        self.data = None 
        self.dataParser()


#@ based on biological file type provides header file
    def fileHeader(self):
        if self.fileType == "gtf" or self.fileType == "gff":
            self.column_names = ["Seqname","Source","feature","Start","End","score","strand","frame","attribute"]
        elif self.fileType == "bed":
            self.column_names = []
        else:
            self.column_names = []

#@ retrive index of Documneted/commented lines
    def retriveCommentIndex(self):
        indx = []
        self.fileobj.seek(0)
        for ix,l in enumerate(self.fileobj):
            if l[0] == '#':
                indx.append(ix)
        self.fileobj.seek(0)
        return indx

#@ get MetaData from the file 
    def getMetaData(self):
        skip_index = self.retriveCommentIndex()
        filetxt = self.fileobj.readlines()
        for lineidx in skip_index:
            print(filetxt[lineidx],end="")
        self.fileobj.seek(0)

#@ Transforming attribute data into columns
    def attributeDataTransform(self,stng,v):
        temp_dict = {}
        for i in stng:
            i = i.strip().split(v)
            if len(i) == 1:
                if i[0] != " ":
                    temp_dict["extra"] = i[0].strip()
                else:
                    pass
            else:
                temp_dict[i[0].strip()] = i[1].strip()
        return temp_dict

#@ new Reads the content of GTF/GFF file into a pandas dataframe
    def dataParser(self):
        self.data = pd.read_csv(self.fileobj,sep='\t',header=None,names=self.column_names,comment='#')
        # Transforming attribute column values to columns takes time 
        newinfo = pd.DataFrame.from_dict(dict(self.data["attribute"].str.split(self.colSep).apply(self.attributeDataTransform,v=self.valSep))).T
        self.data = self.data.join(newinfo,how="left")
        self.data.drop("attribute",axis=1,inplace=True)
    
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
            feature_data = self.data[self.data["feature"] == features]
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

#@ Gives general information on GTF/GFF file   
    def generalInfo(self):
        seqPerCount = pd.DataFrame(self.data['Seqname'].value_counts())
        print(f"GTF/GFF source: {self.data['Source'].unique}")
        print(f"No. of sequence set: {len(seqPerCount.index)}")
        print(f"Sequence Set: {list(seqPerCount.index)}")
        print(seqPerCount)



#@ main body
filename= "sample.gtf"
bioFileType = "gtf"
# These values are for attribute column 
columValue_Seperator = ";"
valueKey_Seperator = " "
gffp = tabFileReader(filename,bioFileType,columValue_Seperator,valueKey_Seperator)

# EDA on data
print(gffp.data.describe)
# getting 
print(gffp.data.info)
# getting information on columns of the dataset
print(gffp.data.dtypes)

# Fetch all the commented lines from the file 
gffp.getMetaData()

# Fetch all the unique features from the table 
print(f"Unique Features in GFF : {filename}")
for i in gffp.data["feature"].unique(): 
    print(f"  {i}")


# Fetch number of records per feature
print(f"Number of records per features: ")
feat_rec = gffp.data["feature"].value_counts()
print(feat_rec)

# Fetch records from features

# gffp.showgffFeatures(['CDS','mRNA'])
# data=gffp.getDataWithSingleFeature('mRNA')
# data=gffp.getDataWithMultipleFeatures(['mRNA','CDS'])
# print(gffp.removeColumn(data,'Parent'))
# gffp.dataToFile(gffp.getDataWithSingleFeature('mRNA'),"output2.tsv")
# gffp.dataToFile(gffp.getDataWithMultipleFeatures(['exon','CDS']),"output.tsv")
