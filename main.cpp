#include <iostream>
#include <vector>
#include "Image.hpp"


// g++ -std=c++17 -Wall -Wextra -O2 Image.cpp main.cpp -o test_image.exe
// .\test_image.exe


int main()
{
    try {
        Image img0;
        std::cout << "img0: " << img0 << std::endl;

        Image img1(4, 3, 3, "RGB", 10);
        std::cout << "img1: " << img1 << std::endl;

        img1.setPixel(0, 0, 0, 100);
        img1(1, 0, 1) = 150;
        std::cout << "img1(0,0,0)=" << (int)img1.getPixel(0,0,0) << std::endl;

        // Opérations avec valeur
        Image brighter = img1 + 50;
        std::cout << "brighter: " << brighter << std::endl;

        // Opérations avec pixel
        std::vector<unsigned char> red(3);
        red[0] = 50; red[1] = 0; red[2] = 0;
        Image moreRed = img1 + red;
        std::cout << "moreRed: " << moreRed << std::endl;

        // Inversion
        Image inverted = ~img1;
        std::cout << "inverted: " << inverted << std::endl;

        // Seuillage
        Image thresh = img1 > 100;
        std::cout << "thresh: " << thresh << std::endl;

        // Deux images tailles différentes
        Image img2(2, 2, 3, "RGB", 200);
        Image sum = img1 + img2;
        std::cout << "sum: " << sum << std::endl;

        // --- NOUVEAUX TESTS ---

        // 1) operator- et operator^ entre deux images
        Image diff = img1 - img2;
        std::cout << "diff (img1 - img2): " << diff << std::endl;

        Image diffAbs = img1 ^ img2;
        std::cout << "diffAbs (img1 ^ img2): " << diffAbs << std::endl;

        // 2) Test de division (img1 / 2.0)
        Image half = img1 / 2.0;
        std::cout << "half (img1 / 2.0): " << half << std::endl;

        // 3) Test d’exception (modèles différents) avec try/catch local
        try {
            Image gray(4, 3, 1, "GRAY", 100);
            Image bad = img1 + gray; // doit lancer une exception
            std::cout << "bad (RGB + GRAY): " << bad << std::endl; // ne devrait pas s'afficher
        } catch (const std::exception& e) {
            std::cout << "Exception attendue (formats différents): " << e.what() << std::endl;
        }

        // Sauvegarde / chargement
        img1.save("test.imgbin");
        Image imgLoaded;
        imgLoaded.load("test.imgbin");
        std::cout << "imgLoaded: " << imgLoaded << std::endl;

        // Test d’exception sur coordonnées (décommenter pour tester)
        // unsigned char v = img1.at(100, 100, 0); // devrait lancer une exception

    } catch (const std::exception& e) {
        std::cerr << "Exception (globale): " << e.what() << std::endl;
    }

    return 0;
}