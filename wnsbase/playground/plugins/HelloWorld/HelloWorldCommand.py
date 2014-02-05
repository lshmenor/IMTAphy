# We will provide a command.
import wnsbase.playground.plugins.Command

class HelloWorldCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        """ Initialize the plugin.
        """

        # We prepare the usage and rationale strings

        usage = "\n%prog helloworld\n\n"
        rationale = "Want to write your own commands? Here is HelloWorld for playground"

        usage += rationale

        usage += """

helloworld will say hello to you. In addition it will echo the string you define
with the --echo switch.
"""
        # Finish constructing the command by calling the base constructor
        # helloworld will be the name of the command

        # The rationale is shown in the global help screen along with the commands name
        # The rest of the usage distription will be shown if you do playground helloworld --help
        wnsbase.playground.plugins.Command.Command.__init__(self, "helloworld", rationale, usage)


        # If you have any options for your plugin you can add them to
        # wnsbase.playground.plugins.Command.Command

        self.addOption("", "--echo",
                       type="string", dest = "echo", metavar = "STRING", default = "DefaultEcho",
                       help = "HelloWorld echoes the string to the user when invoked.")

    # If you need to setup something before execution
    # Implement startup

    def startup(self, args):
        # Make sure you call startup of the  base class
        # wnsbase.playground.plugins.Command.Command take the commandline arguments and
        # passes them to a internal optparse.optionParser . If you do not forward the
        # startup call you will not have an optionParser available
        wnsbase.playground.plugins.Command.Command.startup(self, args)

        # Place your startup code here

    # This will be called by playground when the user issues 'playground.py helloworld'
    def run(self):
        print
        print "Hello World."
        print
        print "You said I should echo : %s" % self.options.echo
        print
        print "This demo plugin can be found in wnsbase/playground/plugins/HelloWorld"


    def shutdown(self):
        # Is called after command execution
        # Place your shutdown code here

        # Then call base class
        wnsbase.playground.plugins.Command.Command.shutdown(self)

