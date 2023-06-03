#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
using namespace std;

//moj algoritam

void draw(int n[20][40]) {
    system("cls");
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 40; j++)
            switch (n[i][j]) {
            case 0: cout << "*"; break;
            case 1:cout << "A"; break;
            case 2:cout << "B"; break;
            case 3:cout << "X"; break;
            }
        cout << endl;
    }
}

int main(void) {
    int n[20][40]{};
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 40; j++)
            n[i][j] = 0;
    int xa, ya, xb, yb;
    cout << "Enter A[X] position: ";
    cin >> xa;
    cout << "Enter A[Y] position: ";
    cin >> ya;
    n[--xa][--ya] = 1;
    cout << "Enter B[X] position: ";
    cin >> xb;
    cout << "Enter B[Y] position: ";
    cin >> yb;
    n[--xb][--yb] = 2;
    int coords[4] = { xa,ya,xb,yb };
    do {
        draw(n);
        if (n[coords[0]][coords[1]] == 3)
            n[coords[0]][coords[1]] = 0;
        if (coords[0] < coords[2])
            ++coords[0];
        if (coords[1] < coords[3])
            ++coords[1];
        if (coords[0] > coords[2])
            --coords[0];
        if (coords[1] > coords[3])
            --coords[1];
        n[coords[0]][coords[1]] = 3;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    } while (!(coords[0] == coords[2] && coords[1] == coords[3]));
    draw(n);

    return 0;
}