$env:PATH = "C:\Users\yewil\.gemini\tools\mingw64\bin;" + $env:PATH
Write-Host "Compilateur utilisé :"
g++ --version
Write-Host "`nCompilation en cours..."
g++ -std=c++17 -Wall -Wextra -O2 Image.cpp main.cpp -o test_image.exe
if ($?) {
    Write-Host "Compilation réussie ! Lancement du programme...`n" -ForegroundColor Green
    ./test_image.exe
}
else {
    Write-Host "Erreur de compilation." -ForegroundColor Red
}