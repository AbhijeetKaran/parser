from collections import Counter
import string

class FastaParser:
    def __init__(self,filename):
        self.filename = filename
        self.metadata = getData(self.filename)
        self.fasta_type = self.getFastaType()
    
    # checks integrity of sequences in the fasta file
    def checkSeqIntegerity(self):
        alphaset = [x for x in string.ascii_uppercase]
        del alphaset[0]
        del alphaset[1]
        del alphaset[4]
        del alphaset[10]
        del alphaset[15]
        each_header = {}
        for headers in self.metadata.keys():
            sequence = self.getSequence(headers)[1].upper()
            character_set = Counter(sequence)
            for seq_c in character_set.keys():
                if seq_c in alphaset:
                    each_header[headers] = "corrupted"
                    break
        if len(each_header.keys()) > 0:
            return "fail"
        else:
            return "pass"

    # Get fasta file type
    def getFastaType(self):
        if len(self.metadata.keys()) > 1:
            return "single"
        else:
            return "multi"

    # Gets sequence based on header
    def getSequence(self,header):
        s_loc = self.metadata[header].split(':')
        sequence = "" 
        fh = open(self.filename,'r')
        fh.seek(int(s_loc[0]))
        current_p = fh.tell()
        lines = fh.readline().strip()
        while current_p < int(s_loc[1]):
            sequence=sequence+lines
            lines = fh.readline().strip()
            current_p = fh.tell()
        fh.close()
        return [header,sequence]

    # Removes record
    def removeSequenceRecord(self,headerName):
        del self.metadata[headerName]

    # Get sequence with masking
    def maskSequence(self,header,start,end):
        sequence = getSequence(header)[1].upper()
        mask = 'N'*(end-start)
        maskedSequence = sequence[:start]+mask+sequence[end:]
        return maskedSequence



    # Writes data into a new Fasta file
    def writeFasta(self,newfilename):
        fhw = open(newfilename,'w')
        for headers in self.metadata.keys():
            sequence = self.getSequence(headers)[1].upper()
            fhw.write(headers)
            fhw.write("\n")
            fhw.write(sequence)
            fhw.write("\n")
        fhw.close()
        print("file created : ",newfilename)


# parse the file and gets the sequence header and sequence info
def getData(filename):
    fh = open(filename,'r')
    n_header = False
    sequence_set = {}
    s_loc = ""
    start_p = 0
    current_p = 0
    lines = fh.readline()
    current_p = fh.tell()
    while lines:
        if lines.strip() == "":
            continue
        if lines[0] == '>':
            if n_header == True:
                s_loc = str(start_p)+":"+str(current_p-1)
                sequence_set[header] = s_loc
                header = lines.strip().replace('>','')
                start_p = current_p
            else:
                n_header = True
                header = lines.strip().replace('>','')
                start_p = current_p
        else:
            l = len(lines)
        lines = fh.readline()
        current_p = fh.tell()
    s_loc = str(start_p)+":"+str(start_p+l-1)
    sequence_set[header] = s_loc
    return sequence_set


#<<:: main body ::>>
filename = "seq.fa"
fp = FastaParser(filename)
print(fp.metadata)
print(fp.getFastaType())
print(fp.getSequence('header2'))
print(fp.checkSeqIntegerity())
fp.removeSequenceRecord('header1')
print(fp.metadata)
fp.writeFasta("newFasta.fa")

