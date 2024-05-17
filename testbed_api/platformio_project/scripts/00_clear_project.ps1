
# Script for deleting project folder

$PROJECT_DIR = "..\project"

Remove-Item -LiteralPath $PROJECT_DIR -Verbose -Recurse
"Project folder deleted."
