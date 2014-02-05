import wnsbase.playground.Core
import PyCoDocumentation

core = wnsbase.playground.Core.getCore()

if not core.hasPlugin("PyCoDocumentation"):
    core.registerPlugin("PyCoDocumentation")

    pyCoDocumentationCommand = PyCoDocumentation.PyCoDocumentationCommand()

    core.registerCommand(pyCoDocumentationCommand)
