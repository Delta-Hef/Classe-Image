#include <iostream>
#include <vector>
#include "Image.hpp"

// .\compile_and_run.ps1
// g++ -std=c++17 -Wall -Wextra -O2 Image.cpp main.cpp -o test_image.exe
// .\test_image.exe

// Petit helper pour afficher un pixel (tous les canaux)
void printPixel(const Image& img, int x, int y, const std::string& label)
{
    std::cout << label << " (x=" << x << ", y=" << y << ") = [ ";
    for (int c = 0; c < img.getChannels(); ++c) {
        std::cout << (int)img.getPixel(x, y, c);
        if (c + 1 < img.getChannels()) std::cout << ", ";
    }
    std::cout << " ]\n";
}

int main()
{
    try {
        std::cout << "CONSTRUCTION D'IMAGES\n";

        Image img0;
        std::cout << "[img0] Image par defaut : " << img0 << "\n\n";

        Image img1(4, 3, 3, "RGB", 10);
        std::cout << "[img1] Image RGB 4x3 remplie avec 10 : " << img1 << "\n";

        img1.setPixel(0, 0, 0, 100);
        img1(1, 0, 1) = 150;
        printPixel(img1, 0, 0, "  -> img1 pixel avant operations");
        std::cout << "\n";


        std::cout << "OPERATIONS AVEC SCALAIRE (int/double)\n";

        Image brighter = img1 + 50;
        std::cout << "[brighter] img1 + 50 (luminosite +50)\n";
        printPixel(img1,     0, 0, "   avant  (img1)");
        printPixel(brighter, 0, 0, "   apres  (brighter)");
        std::cout << "\n";

        Image subVal = img1 - 50;
        std::cout << "[subVal] img1 - 50 (luminosite -50)\n";
        printPixel(img1,   0, 0, "   avant  (img1)");
        printPixel(subVal, 0, 0, "   apres  (subVal)");
        std::cout << "\n";

        Image diffVal = img1 ^ 50;
        std::cout << "[diffVal] img1 ^ 50 (|pixel - 50|)\n";
        printPixel(img1,    0, 0, "   avant  (img1)");
        printPixel(diffVal, 0, 0, "   apres  (diffVal)");
        std::cout << "\n";

        Image mult = img1 * 1.5;
        std::cout << "[mult] img1 * 1.5\n";
        printPixel(img1, 0, 0, "   avant  (img1)");
        printPixel(mult, 0, 0, "   apres  (mult)");
        std::cout << "\n";

        Image half = img1 / 2.0;
        std::cout << "[half] img1 / 2.0\n";
        printPixel(img1, 0, 0, "   avant  (img1)");
        printPixel(half, 0, 0, "   apres  (half)");
        std::cout << "\n";


        std::cout << "OPERATIONS AVEC VECTEUR PIXEL (std::vector<unsigned char>)\n";

        std::vector<unsigned char> red(3);
        red[0] = 50; red[1] = 0; red[2] = 0;
        Image moreRed = img1 + red;
        std::cout << "[moreRed] img1 + [50,0,0] (ajout de rouge)\n";
        printPixel(img1,    0, 0, "   avant  (img1)");
        printPixel(moreRed, 0, 0, "   apres  (moreRed)");
        std::cout << "\n";

        std::vector<unsigned char> px(3);
        px[0] = 10; px[1] = 20; px[2] = 30;

        Image subPix = img1 - px;
        std::cout << "[subPix] img1 - [10,20,30]\n";
        printPixel(img1,  0, 0, "   avant  (img1)");
        printPixel(subPix,0, 0, "   apres  (subPix)");
        std::cout << "\n";

        Image diffPix = img1 ^ px;
        std::cout << "[diffPix] img1 ^ [10,20,30]\n";
        printPixel(img1,   0, 0, "   avant  (img1)");
        printPixel(diffPix,0, 0, "   apres  (diffPix)");
        std::cout << "\n";


        std::cout << "INVERSION ET SEUILLAGE\n";

        Image inverted = ~img1;
        std::cout << "[inverted] ~img1 (255 - valeur)\n";
        printPixel(img1,    0, 0, "   avant  (img1)");
        printPixel(inverted,0, 0, "   apres  (inverted)");
        std::cout << "\n";

        Image thresh = img1 > 100;
        std::cout << "[thresh] img1 > 100 (GRAY 0/255)\n";
        // Seuillage : l'image resultat a 1 canal, donc on affiche canal 0
        std::cout << "   pixel (0,0) resultat = [ "
                  << (int)thresh.getPixel(0,0,0) << " ]\n\n";


        std::cout << "OPERATIONS ENTRE DEUX IMAGES (img1 et img2)\n";

        Image img2(2, 2, 3, "RGB", 200);
        std::cout << "[img2] Image RGB 2x2 remplie avec 200\n";
        printPixel(img1, 0, 0, "   img1 (0,0)");
        printPixel(img2, 0, 0, "   img2 (0,0)");

        Image sum = img1 + img2;
        std::cout << "[sum] img1 + img2\n";
        printPixel(sum, 0, 0, "   sum  (0,0)");
        std::cout << "\n";

        Image diff = img1 - img2;
        std::cout << "[diff] img1 - img2\n";
        printPixel(diff, 0, 0, "   diff (0,0)");
        std::cout << "\n";

        Image diffAbs = img1 ^ img2;
        std::cout << "[diffAbs] img1 ^ img2\n";
        printPixel(diffAbs, 0, 0, "   diffAbs (0,0)");
        std::cout << "\n";


        std::cout << "TEST D'EXCEPTION DE FORMAT (RGB vs GRAY)\n";
        try {
            Image gray(4, 3, 1, "GRAY", 100);
            std::cout << "   tentative img1 + gray...\n";
            Image bad = img1 + gray; // doit lancer
            std::cout << "   ERREUR: pas d'exception ! bad = " << bad << "\n";
        } catch (const std::exception& e) {
            std::cout << "   Exception attendue : " << e.what() << "\n\n";
        }

        std::cout << "SAUVEGARDE / CHARGEMENT\n";
        img1.save("test.imgbin");
        Image imgLoaded;
        imgLoaded.load("test.imgbin");
        std::cout << "   imgLoaded = " << imgLoaded << "\n";

        std::cout << "FIN DES TESTS\n";

    } catch (const std::exception& e) {
        std::cerr << "[ERREUR GLOBALE] " << e.what() << "\n";
    }

    return 0;
}