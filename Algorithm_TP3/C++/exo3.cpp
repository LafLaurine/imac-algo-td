#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w = nullptr;
using std::size_t;

struct BinarySearchTree : public BinaryTree
{
	BinarySearchTree(int value=0) : BinaryTree(value) {}
	virtual ~BinarySearchTree() {}

	void insertNumber(int value)
    {
        if(this->value == NULL || this->value == value) {
            createNode(value);
        } else if(this->value < value){
            if(this->right == NULL){
                this->right = createNode(value);
               } else{
                this->right->insertNumber(value);
            }
            }
        else if(this->value > value) {
             if(this->left == NULL){
                 this->left = createNode(value);
             } else{
                 this->left->insertNumber(value);
             }
        }
    }

    uint height() const
	{
        uint height = 1;
        int max = 0;
        if((this->left) != NULL){
            max = this->left->height();
        }
        if((this->right) != NULL){
            if(max<this->right->height()){
                    max = this->right->height();
            }
        }
        return height+max;
	}

	uint nodesCount() const
	{
		uint count = 1;
        int size = 0;
        if((this->left) != NULL){
            size = this->left->nodesCount();
        }
        if((this->right) != NULL){
            if(this->right){
                    size = size+this->right->nodesCount();
            }
        }
        return count+size;
	}

	bool isLeaf() const
    {
        if(this->left == nullptr && this->right == nullptr){
            return true;
        }
		return false;
	}

	void allLeaves(Node* leaves[], uint& leavesCount)
    {
        if(this->isLeaf()){
            leaves[leavesCount] = this;
            leavesCount++;
        }else {
            if(this->left != NULL){
               this->left->allLeaves(leaves, leavesCount);
            }
            if(this->right != NULL){
                this->right->allLeaves(leaves, leavesCount);
            }
        }
	}

	void inorderTravel(Node* nodes[], uint& nodesCount)
	{
        if(this->left){
            (this->left)->inorderTravel(nodes, nodesCount);
        }

	}

	void preorderTravel(Node* nodes[], uint& nodesCount)
	{

	}

	void postorderTravel(Node* nodes[], uint& nodesCount)
	{

	}

	Node* find(int value)
	{

	}
};

Node* createNode(int value) {
	return new BinarySearchTree(value);
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow::instruction_duration = 200;
	BinarySearchTree bst;
	w = new BinarySearchTreeWindow(bst);
	w->show();

	return a.exec();
}
