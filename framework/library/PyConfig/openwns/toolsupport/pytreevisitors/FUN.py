import openwns.toolsupport
import openwns.FUN

import subprocess
import os

class FUNVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return o.__class__.__module__ + "." + o.__class__.__name__

    def renderLongText(self, o, pathToObject):
        try:
            o.dot('__tmp.dot', showParameters=False)
            subprocess.check_call(["dot", "__tmp.dot", "-T", "jpg", "-o__pytree_tmp.jpg"])
            pid = subprocess.Popen(["/usr/bin/eog", "__pytree_tmp.jpg"]).pid
        finally:
            pass
            if os.path.exists("__tmp.dot"):
                subprocess.check_call(["rm", "-f", "__tmp.dot"])
        return "A graphic viewer should pop up and display the Functional Unit Network.\n If you do not see one please contact us.\n"

openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.FUN.FUN, FUNVisitor())
