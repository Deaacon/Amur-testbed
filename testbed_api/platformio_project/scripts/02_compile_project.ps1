
# Script for source compilation

$PROJECT_DIR = "..\project"
$CURRENT_DIR = Get-Location
Set-Location $PROJECT_DIR

"Clear previous compilation."
pio run --target clean

"Start compilation."
pio run

Set-Location $CURRENT_DIR

"Compilation finished."
