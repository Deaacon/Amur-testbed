
# Script for project initialization

$PROJECT_DIR = "..\project"

New-Item -Path $PROJECT_DIR -ItemType "directory"
Copy-Item -Path ..\platformio.ini -Destination $PROJECT_DIR
Copy-Item -Path ..\src -Destination $PROJECT_DIR -Recurse

"Folder project created."
