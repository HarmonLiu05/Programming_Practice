import docx2txt
text = docx2txt.process("partC(2).docx")
open("partC2.txt","w",encoding="utf-8").write(text)
print("OK")
