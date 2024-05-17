
# Script for hex to board uploading

$PROJECT_DIR = "..\project"
$CURRENT_DIR = Get-Location
Set-Location $PROJECT_DIR

"Start target uploading."
pio run --target upload

Set-Location $CURRENT_DIR

"Target upload finished."
