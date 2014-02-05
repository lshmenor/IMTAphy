from wx.html import HtmlEasyPrinting

class Printer(HtmlEasyPrinting):
    def __init__(self):
        HtmlEasyPrinting.__init__(self)

    def GetHtmlText(self,text):
        "Simple conversion of text.  Use a more powerful version"
        html_text = text.replace('\n\n','<P>')
        html_text = text.replace('\n', '<BR>')
        return html_text

    def Print(self, text, doc_name):
        self.SetHeader(doc_name)
        self.SetFooter("<center>Page @PAGENUM@/@PAGESCNT@ - Created by <em><font color=\"#FF6600\">open</font></em><font color=\"#333333\">WNS</font> PyTree on @DATE@</center>")
        self.PrintText(self.GetHtmlText(text),doc_name)

    def PreviewText(self, text, doc_name):
        self.SetHeader(doc_name)
        HtmlEasyPrinting.PreviewText(self, self.GetHtmlText(text))
