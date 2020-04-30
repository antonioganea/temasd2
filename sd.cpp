#include <iostream>
#include <queue>
using namespace std;

#define COLOR_BLACK 0
#define COLOR_RED 1

class RBNode {
public:
    int value;
    bool color;
    RBNode * left, * right, * parent;

    RBNode(int _value) {
        value = _value;
        parent = left = right = NULL;
        color = COLOR_RED;
    }

    RBNode* uncle() {
        if (parent == NULL or parent->parent == NULL)
            return NULL;

        if (parent->isOnLeft())
            return parent->parent->right;
        else
            return parent->parent->left;
    }

    bool isOnLeft() {
        return this == parent->left;
    }

    RBNode* sibling() {
        if (parent == NULL)
            return NULL;

        if (isOnLeft())
            return parent->right;

        return parent->left;
    }

    void moveDown(RBNode* nParent) {
        if (parent != NULL) {
            if (isOnLeft()) {
                parent->left = nParent;
            } else {
                parent->right = nParent;
            }
        }
        nParent->parent = parent;
        parent = nParent;
    }

    bool hasREDChild() {
        return (left != NULL and left->color == COLOR_RED) or
                     (right != NULL and right->color == COLOR_RED);
    }
};

class RBTree {
private:
    RBNode* root;

public:
    void leftRotate(RBNode *x) {

        RBNode* nParent = x->right;

        if (x == root)
            root = nParent;

        x->moveDown(nParent);

        x->right = nParent->left;

        if (nParent->left != NULL)
            nParent->left->parent = x;

        nParent->left = x;
    }

    void rightRotate(RBNode *x) {
        RBNode* nParent = x->left;

        if (x == root)
            root = nParent;

        x->moveDown(nParent);

        x->left = nParent->right;

        if (nParent->right != NULL)
            nParent->right->parent = x;

        nParent->right = x;
    }

    void swapColors(RBNode* x1, RBNode* x2) {
        bool temp;
        temp = x1->color;
        x1->color = x2->color;
        x2->color = temp;
    }

    void swapValues(RBNode* u, RBNode* v) {
        int temp;
        temp = u->value;
        u->value = v->value;
        v->value = temp;
    }

    void fixRED(RBNode* x) {
        if (x == root) {
            x->color = COLOR_BLACK;
            return;
        }

        RBNode* parent = x->parent;
        RBNode* grandparent = parent->parent;
        RBNode* uncle = x->uncle();

        if (parent->color != COLOR_BLACK) {
            if (uncle != NULL && uncle->color == COLOR_RED) {
                parent->color = COLOR_BLACK;
                uncle->color = COLOR_BLACK;
                grandparent->color = COLOR_RED;
                fixRED(grandparent);
            } else {
                if (parent->isOnLeft()) {
                    if (x->isOnLeft()) {
                        swapColors(parent, grandparent);
                    } else {
                        leftRotate(parent);
                        swapColors(x, grandparent);
                    }
                    rightRotate(grandparent);
                } else {
                    if (x->isOnLeft()) {
                        rightRotate(parent);
                        swapColors(x, grandparent);
                    } else {
                        swapColors(parent, grandparent);
                    }
                    leftRotate(grandparent);
                }
            }
        }
    }

    RBNode* successor(RBNode *x) {
        RBNode* temp = x;
        while (temp->left != NULL)
            temp = temp->left;
        return temp;
    }

    RBNode* BSTreplace(RBNode *x) {
        if (x->left != NULL and x->right != NULL)
            return successor(x->right);

        if (x->left == NULL and x->right == NULL)
            return NULL;

        if (x->left != NULL)
            return x->left;
        else
            return x->right;
    }

    void deleteNode(RBNode *v) {
        RBNode* u = BSTreplace(v);

        bool uvBLACK = ((u == NULL or u->color == COLOR_BLACK) and (v->color == COLOR_BLACK));
        RBNode* parent = v->parent;

        if (u == NULL) {
            if (v == root) {
                root = NULL;
            } else {
                if (uvBLACK) {
                    fixDoubleBlack(v);
                } else {
                    if (v->sibling() != NULL)
                        v->sibling()->color = COLOR_RED;
                }

                if (v->isOnLeft()) {
                    parent->left = NULL;
                } else {
                    parent->right = NULL;
                }
            }
            delete v;
            return;
        }

        if (v->left == NULL or v->right == NULL) {
            if (v == root) {
                v->value = u->value;
                v->left = v->right = NULL;
                delete u;
            } else {
                if (v->isOnLeft()) {
                    parent->left = u;
                } else {
                    parent->right = u;
                }
                delete v;
                u->parent = parent;
                if (uvBLACK) {
                    fixDoubleBlack(u);
                } else {
                    u->color = COLOR_BLACK;
                }
            }
            return;
        }

        swapValues(u, v);
        deleteNode(u);
    }

    void fixDoubleBlack(RBNode *x) {
        if (x == root)
            return;

        RBNode* sibling = x->sibling(), *parent = x->parent;
        if (sibling == NULL) {
            fixDoubleBlack(parent);
        } else {
            if (sibling->color == COLOR_RED) {
                parent->color = COLOR_RED;
                sibling->color = COLOR_BLACK;
                if (sibling->isOnLeft()) {
                    rightRotate(parent);
                } else {
                    leftRotate(parent);
                }
                fixDoubleBlack(x);
            } else {
                if (sibling->hasREDChild()) {
                    if (sibling->left != NULL and sibling->left->color == COLOR_RED) {
                        if (sibling->isOnLeft()) {
                            sibling->left->color = sibling->color;
                            sibling->color = parent->color;
                            rightRotate(parent);
                        } else {
                            sibling->left->color = parent->color;
                            rightRotate(sibling);
                            leftRotate(parent);
                        }
                    } else {
                        if (sibling->isOnLeft()) {
                            sibling->right->color = parent->color;
                            leftRotate(sibling);
                            rightRotate(parent);
                        } else {
                            sibling->right->color = sibling->color;
                            sibling->color = parent->color;
                            leftRotate(parent);
                        }
                    }
                    parent->color = COLOR_BLACK;
                } else {
                    sibling->color = COLOR_RED;
                    if (parent->color == COLOR_BLACK)
                        fixDoubleBlack(parent);
                    else
                        parent->color = COLOR_BLACK;
                }
            }
        }
    }

public:
    RBTree() { root = NULL; }

    RBNode* search(int n) {
        RBNode *temp = root;
        while (temp != NULL) {
            if (n < temp->value) {
                if (temp->left == NULL)
                    break;
                else
                    temp = temp->left;
            } else if (n == temp->value) {
                break;
            } else {
                if (temp->right == NULL)
                    break;
                else
                    temp = temp->right;
            }
        }

        return temp;
    }

    void insert(int n) {
        RBNode* newNode = new RBNode(n);
        if (root == NULL) {
            newNode->color = COLOR_BLACK;
            root = newNode;
        } else {
            RBNode* temp = search(n);

            if (temp->value == n) {
                return;
            }

            newNode->parent = temp;

            if (n < temp->value)
                temp->left = newNode;
            else
                temp->right = newNode;

            fixRED(newNode);
        }
    }

    void remove(int value) {
        if (root == NULL)
            return;

        RBNode* node = search(value);

        if (node->value != value) {
            //No node found
            return;
        }

        deleteNode(node);
    }

    RBNode* succesor(int val)
    {
        RBNode* last = 0;
        RBNode* node = root;
        while ( node != NULL ) {
            if (node->value == val)
                return node;
            else if (val < node->value) {
                last = node;
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return last;
    }

    RBNode* predecesor(int val) {
        RBNode* last = 0;
        RBNode* node = root;
        while (node != NULL) {
            if (node->value == val)
                return node;

            else if (val < node->value) {
                node = node->left;
            }
            else {
                last = node;
                node = node->right;
            }
        }
        return last;
    }

    void printInterval(int left, int right) {
        RBNode* current = succesor(left);
        while ( current != NULL && current->value <= right )
        {
            printf("%d ",current->value);
            current = succesor(current->value + 1);
        }
        puts("");
    }
};

FILE * fin;
FILE * fout;

int main() {
    RBTree mytree;

    fin = fopen("abce.in","r");
    freopen ("abce.out","w",stdout);

    int N;

    fscanf(fin, "%d", &N);

    for ( int i = 0; i < N; i++ ){
        int command;
        fscanf(fin,"%d",&command);
        bool cond;

        if ( command == 1 ){
            int toInsert;
            fscanf(fin, "%d", &toInsert);
            mytree.insert(toInsert);
            continue;
        }

        if ( command == 2 ){
            int toDelete;
            fscanf(fin, "%d", &toDelete);
            mytree.remove(toDelete);
            continue;
        }

        if ( command == 3 ){
            int toSearch;
            fscanf(fin, "%d", &toSearch);
            RBNode * searchedNode = mytree.search(toSearch);
            if ( searchedNode ){
                int n = searchedNode->value;
                cond = ( n == toSearch);
                printf("%d\n", cond);
            }else {
                printf("%d\n",0);
            }
            continue;
        }
        
        if ( command == 4 ){
            int Y;
            fscanf(fin,"%d",&Y);

            RBNode* succesor = mytree.predecesor(Y);
            if ( succesor ){
                printf("%d\n", succesor->value);
            } else {
                puts("-1");
            }
            continue;
        }

        if ( command == 5 ){
            int Y;
            fscanf(fin,"%d",&Y);
            RBNode* succesor = mytree.succesor(Y);
            if ( succesor ){
                printf("%d\n", succesor->value);
            } else {
                puts("-1");
            }
            continue;
        }

        if ( command == 6 ){
            int X, Y;
            fscanf(fin,"%d%d",&X,&Y);
            mytree.printInterval(X,Y);
            continue;
        }

    }
    fclose(stdout);
    return 0;
}
