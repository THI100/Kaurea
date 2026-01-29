#use 'chmod +x scripts/activate_testvector.sh' to enable running the file.

echo "Compiling..."
gcc -O3 -march=native -funroll-loops src/flow.c src/formulas.c src/kaurea.c src/solutions.c src/main.c -o Kaurea -lm
echo "Run: Kaurea.exe for windows or ./Kaurea for linux/mac"