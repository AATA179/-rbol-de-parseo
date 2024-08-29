#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <GL/glut.h>
#include <regex>

using namespace std;

#define M_PI 3.14159265358979323846 // Definición de PI para dibujar los circulos del arbol

// Contenedores globales para separar los tokens y valores numéricos
vector<string> cadena;
vector<string> valores;

// Estructura para los nodos del árbol ternario
struct Nodo {
    string valor;
    Nodo* izquierdo;
    Nodo* medio;
    Nodo* derecho;
    Nodo* padre;

    // Constructor del nodo
    Nodo(const string& val) : valor(val), izquierdo(nullptr), medio(nullptr), derecho(nullptr), padre(nullptr) {}
};

// Función para verificar si una cadena representa un número
bool esNumero(const string& str) {
    std::regex numero_regex("^[0-9]+$"); // Expresión regular para números enteros
    return regex_match(str, numero_regex);
}

// Clase para manejar un árbol ternario
class ArbolTernario {
public:
    Nodo* raiz; // Nodo raíz del árbol

    // Constructor
    ArbolTernario() : raiz(nullptr) {}

    // Inserta un nuevo nodo en el árbol
    void insertar(const string& val, Nodo*& nodo, Nodo* padre = nullptr) {
        if (nodo == nullptr) {
            nodo = new Nodo(val);
            nodo->padre = padre;
        }
    }

    // Calcula la altura del árbol
    int altura(Nodo* raiz) {
        if (raiz == nullptr) {
            return 0;
        }
        else {
            int alturaIzquierda = altura(raiz->izquierdo);
            int alturaMedia = altura(raiz->medio);
            int alturaDerecha = altura(raiz->derecho);
            return 1 + max({ alturaIzquierda, alturaMedia, alturaDerecha });
        }
    }

    // Dibuja una línea entre dos puntos
    void dibujarLinea(float x1, float y1, float x2, float y2) {
        glColor3f(1.0f, 1.0f, 1.0f); // Color de las líneas (blanco)
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    // Dibuja un nodo en la pantalla
    void dibujarNodo(float x, float y, const std::string& valor) {
        if (valor != "+" && valor != "*" && valor != "(" && valor != ")" && valor != "NULL" && valor != "num" && !esNumero(valor)) {
            // Dibujar un círculo para nodos que no son terminales
            float radius = 0.05f;
            glColor3f(0.0f, 0.0f, 1.0f); // Color del círculo azul
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int i = 0; i <= 100; i++) {
                float angle = 2 * M_PI * i / 100;
                glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
            }
            glEnd();
        }

        // Dibujar el valor del nodo
        glColor3f(1.0f, 1.0f, 1.0f); // Color del texto (blanco)
        glRasterPos2f(x - 0.02f, y - 0.02f);
        for (char c : valor) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    // Dibuja el árbol entero
    void dibujarArbol(Nodo* raiz, float x, float y, float offsetX, float offsetY) {
        if (raiz == nullptr) return;

        // Dibujar líneas hacia los hijos
        if (raiz->izquierdo) {
            float hijoX = x - offsetX;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->izquierdo, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        if (raiz->medio) {
            float hijoX = x;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->medio, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        if (raiz->derecho) {
            float hijoX = x + offsetX;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->derecho, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        // Dibujar el nodo actual
        dibujarNodo(x, y, raiz->valor);
    }

};

// Instancia global del árbol ternario y variable de offset
ArbolTernario arbol;
float offsetX = 0.5f; // Valor inicial del offset en el eje X (Espacio entre nodos del mismo nivel)

// Inicializa la configuración de OpenGL
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Coordenadas ortográficas
}

// Función de visualización de OpenGL
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float x = 0.0f;
    float y = 0.9f;
    float offsetY = 0.2f; // Espacio entre niveles del arbol
    arbol.dibujarArbol(arbol.raiz, x, y, offsetX, offsetY);
    glFlush();
}

/* Función para aumentar o disminuir el espacio entre nodos sobre X
para evitar que los nodos se encimen o se salgan de la pantalla*/
void teclado(unsigned char key, int x, int y) {
    if (key == '+') {
        offsetX += 0.1f; // Aumentar offsetX
    }
    else if (key == '-') {
        offsetX -= 0.1f; // Disminuir offsetX
    }
    glutPostRedisplay(); // Redibuja la ventana
}

// Función para analizar la entrada y tokenizarla
void lexer(const string& entrada) {
    size_t aux = 0;
    string agregar;

    while (aux < entrada.size()) {
        size_t pos = entrada.find_first_of(" +*()", aux);
        if (pos == std::string::npos) {
            agregar = entrada.substr(aux);
            if (!agregar.empty()) {
                cadena.push_back(agregar);
            }
            break;
        }

        if (pos > aux) {
            agregar = entrada.substr(aux, pos - aux);
            cadena.push_back(agregar);
        }

        if (entrada[pos] != ' ') {
            agregar = entrada.substr(pos, 1);
            cadena.push_back(agregar);
        }

        aux = pos + 1;
    }

    /*Si se ingresa un numero se comvierte al token 'num' y el valor del numero lo pasa al 
    vector 'valores' para regresarlos al final del arbol*/
    for (int i = 0; i < cadena.size(); i++) {
        size_t pos2 = cadena[i].find_first_not_of("0123456789");
        if (pos2 == string::npos) {
            valores.push_back(cadena[i]);
            cadena[i] = "num";
        }
    }
}

// Función para construir el árbol a partir de la cadena tokenizada
/*Se manejaron las reglas de produccion de la gramatica a partir de if else*/
void parser() {
    arbol.insertar("<E>", arbol.raiz);
    Nodo* actual = arbol.raiz;

    while (!cadena.empty()) {
        if (actual == nullptr) break;

        if (actual->valor == "<E>") {
            string op;
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "+") {
                    op = "+";
                    break;
                }
                else if (cadena[i] == "*") {
                    op = "*";
                    break;
                }
                else if (cadena[i] == "(") {
                    op = "(";
                    break;
                }
            }

            if (op == "+") {
                arbol.insertar("<E>", actual->izquierdo, actual);
                arbol.insertar("+", actual->medio, actual);
                arbol.insertar("<T>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("<T>", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                actual = actual->medio;
            }
        }
        else if (actual->valor == "<T>") {
            string op;
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "*") {
                    op = "*";
                    break;
                }
                else if (cadena[i] == "(") {
                    op = "(";
                    break;
                }
            }

            if (op == "*") {
                arbol.insertar("<T>", actual->izquierdo, actual);
                arbol.insertar("*", actual->medio, actual);
                arbol.insertar("<U>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("<U>", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                actual = actual->medio;
            }
        }
        else if (actual->valor == "<U>") {
            string op;
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "num") {
                    op = "num";
                    break;
                }
                else if (cadena[i] == "(") {
                    op = "(";
                    break;
                }
            }

            if (op == "(") {
                arbol.insertar("(", actual->izquierdo, actual);
                arbol.insertar("<E>", actual->medio, actual);
                arbol.insertar(")", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                for (size_t j = 0; j < cadena.size(); j++) {
                    if (cadena[j] == ")") {
                        cadena.erase(cadena.begin() + j);
                        break;
                    }
                }
                actual = actual->medio;
            }
            else if (op == "num") {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("num", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);

                if (valores.size() > 0) {
                    arbol.insertar(valores[0], actual->medio->medio);
                    arbol.insertar("NULL", actual->medio->izquierdo);
                    arbol.insertar("NULL", actual->medio->derecho);
                    valores.erase(valores.begin());
                }

                actual = actual->padre;
                while (actual != nullptr) {
                    if (actual->valor == "<T>" && actual->medio != nullptr && actual->medio->valor == "*") {
                        if (actual->derecho != nullptr) {
                            actual = actual->derecho;
                            break;
                        }
                    }
                    else if (actual->valor == "<E>" && actual->medio != nullptr && actual->medio->valor == "+") {
                        if (actual->derecho != nullptr) {
                            actual = actual->derecho;
                            break;
                        }
                    }
                    actual = actual->padre;
                }
            }
        }
    }
}

// Función principal
int main(int argc, char** argv) {
    string entrada;
    cout << "Ingrese la cadena: ";
    getline(cin, entrada);

    lexer(entrada); // Tokenizar la entrada
    parser(); // Analizar y construir el árbol

    // Inicializar OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Arbol de Parseo");

    initOpenGL(); 

    glutDisplayFunc(display); // Función de visualización
    glutKeyboardFunc(teclado); // Función de manejo de teclado

    glutMainLoop(); // Ejecutar el bucle principal de OpenGL

    return 0;
}
