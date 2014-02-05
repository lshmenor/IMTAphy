import openwns.toolsupport
import scenarios.builders

class CreatorPlacerBuilderVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return o.__class__.__name__

    def renderLongText(self, o, pathToObject):
        import matplotlib
        from matplotlib.patches import Circle
        from matplotlib.collections import PatchCollection
        import pylab
        import os

        filename = openwns.toolsupport.createTempfileName() + ".png"

        xpos = []
        ypos = []

        for bspos in o.bsPositions:
            xpos.append(bspos.x)
            ypos.append(bspos.y)

        for utpos in o.utPositions:
            xpos.append(utpos.x)
            ypos.append(utpos.y)

#        pylab.plot((-200,200),(-200.0,200), color='r')
#        pylab.plot((50.0,100.0),(25.0,100.0), color='r')

        xlen = max(xpos) - min(xpos)
        ylen = max(ypos) - min(ypos)

        paddingx = xlen * 0.1
        paddingy = ylen * 0.1

        pylab.xlim(min(xpos) - paddingx, max(xpos) + paddingx)
        pylab.ylim(min(ypos) - paddingy, max(ypos) + paddingy)

        for bspos in o.bsPositions:
            c=Circle((bspos.x, bspos.y), 0.01 * xlen)

            pylab.gca().add_patch(c)

        for utpos in o.utPositions:
            c=Circle((utpos.x, utpos.y), 0.01 * xlen, facecolor="red")

            pylab.gca().add_patch(c)

        pylab.grid()
        pylab.savefig(filename)

        result = "<center><h2>Position of Basestations in scenario</h2><img src=\"%s\"/></center>" % filename

        return result

openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(scenarios.builders.CreatorPlacerBuilder, CreatorPlacerBuilderVisitor())
