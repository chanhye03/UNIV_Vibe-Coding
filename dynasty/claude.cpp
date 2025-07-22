#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// 문자열을 담는 동적 배열 클래스
class StringArray {
private:
    string* data;
    int capacity;
    int size;

public:
    StringArray() : data(nullptr), capacity(0), size(0) {}

    ~StringArray() {
        delete[] data;
    }

    void push_back(const string& str) {
        if (size >= capacity) {
            int newCapacity = (capacity == 0) ? 1 : capacity * 2;
            string* newData = new string[newCapacity];
            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
        data[size++] = str;
    }

    string& operator[](int index) {
        return data[index];
    }

    const string& operator[](int index) const {
        return data[index];
    }

    int getSize() const {
        return size;
    }

    void clear() {
        size = 0;
    }
};

// 트리 노드 클래스
class TreeNode {
public:
    string name;
    TreeNode* parent;
    TreeNode** children;
    int childCount;
    int childCapacity;
    int order; // 왕위 순서

    TreeNode(const string& n) : name(n), parent(nullptr), children(nullptr),
        childCount(0), childCapacity(0), order(-1) {}

    ~TreeNode() {
        delete[] children;
    }

    void addChild(TreeNode* child) {
        if (childCount >= childCapacity) {
            int newCapacity = (childCapacity == 0) ? 1 : childCapacity * 2;
            TreeNode** newChildren = new TreeNode * [newCapacity];
            for (int i = 0; i < childCount; i++) {
                newChildren[i] = children[i];
            }
            delete[] children;
            children = newChildren;
            childCapacity = newCapacity;
        }
        children[childCount++] = child;
        child->parent = this;
    }
};

// 조선왕조 트리 클래스
class JoseonDynastyTree {
private:
    TreeNode* root;
    TreeNode** allNodes;
    int nodeCount;
    int nodeCapacity;
    StringArray kingOrder; // 왕위 순서

    TreeNode* findNode(const string& name) {
        for (int i = 0; i < nodeCount; i++) {
            if (allNodes[i]->name == name) {
                return allNodes[i];
            }
        }
        return nullptr;
    }

    TreeNode* createNode(const string& name) {
        if (nodeCount >= nodeCapacity) {
            int newCapacity = (nodeCapacity == 0) ? 10 : nodeCapacity * 2;
            TreeNode** newNodes = new TreeNode * [newCapacity];
            for (int i = 0; i < nodeCount; i++) {
                newNodes[i] = allNodes[i];
            }
            delete[] allNodes;
            allNodes = newNodes;
            nodeCapacity = newCapacity;
        }
        TreeNode* node = new TreeNode(name);
        allNodes[nodeCount++] = node;
        return node;
    }

    void collectDescendants(TreeNode* node, StringArray& descendants) {
        for (int i = 0; i < node->childCount; i++) {
            descendants.push_back(node->children[i]->name);
            collectDescendants(node->children[i], descendants);
        }
    }

    void collectAncestors(TreeNode* node, StringArray& ancestors) {
        if (node->parent != nullptr) {
            ancestors.push_back(node->parent->name);
            collectAncestors(node->parent, ancestors);
        }
    }

    int getGenerationDistance(TreeNode* ancestor, TreeNode* descendant) {
        if (ancestor == descendant) return 0;

        TreeNode* current = descendant;
        int distance = 0;

        while (current->parent != nullptr) {
            current = current->parent;
            distance++;
            if (current == ancestor) {
                return distance;
            }
        }
        return -1; // 직계 관계가 아님
    }

public:
    JoseonDynastyTree() : root(nullptr), allNodes(nullptr), nodeCount(0), nodeCapacity(0) {}

    ~JoseonDynastyTree() {
        for (int i = 0; i < nodeCount; i++) {
            delete allNodes[i];
        }
        delete[] allNodes;
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            if (line.empty()) continue;

            // 공백으로 분리
            size_t spacePos = line.find(' ');
            string kingName, parentName;

            if (spacePos == string::npos) {
                // 첫 번째 왕 (태조)
                kingName = line;
                parentName = "";
            }
            else {
                kingName = line.substr(0, spacePos);
                parentName = line.substr(spacePos + 1);
            }

            TreeNode* kingNode = findNode(kingName);
            if (kingNode == nullptr) {
                kingNode = createNode(kingName);
            }

            // 왕위 순서 저장
            kingNode->order = kingOrder.getSize();
            kingOrder.push_back(kingName);

            if (!parentName.empty()) {
                TreeNode* parentNode = findNode(parentName);
                if (parentNode == nullptr) {
                    parentNode = createNode(parentName);
                }
                parentNode->addChild(kingNode);
            }
            else {
                root = kingNode; // 태조
            }
        }
        file.close();
    }

    // 1. 조선의 왕을 순서대로 출력
    void printKingsInOrder() {
        cout << "1. 조선의 왕을 순서대로 출력:" << endl;
        for (int i = 0; i < kingOrder.getSize(); i++) {
            cout << kingOrder[i];
            if (i < kingOrder.getSize() - 1) cout << " -> ";
        }
        cout << endl << endl;
    }

    // 2. 조선의 왕을 역순으로 출력
    void printKingsInReverse() {
        cout << "2. 조선의 왕을 역순으로 출력:" << endl;
        for (int i = kingOrder.getSize() - 1; i >= 0; i--) {
            cout << kingOrder[i];
            if (i > 0) cout << " -> ";
        }
        cout << endl << endl;
    }

    // 3. 조선의 왕은 모두 몇 명인가?
    void printTotalKingCount() {
        cout << "3. 조선의 왕은 모두 몇 명인가?" << endl;
        cout << "총 " << kingOrder.getSize() << "명" << endl << endl;
    }

    // 4. 인조의 후손은 누구누구인가?
    void printInjoDescendants() {
        cout << "4. 조선의 왕 중에서 인조의 후손은 누구누구인가?" << endl;
        TreeNode* injo = findNode("인조");
        if (injo != nullptr) {
            StringArray descendants;
            collectDescendants(injo, descendants);

            // 왕만 필터링
            for (int i = 0; i < descendants.getSize(); i++) {
                for (int j = 0; j < kingOrder.getSize(); j++) {
                    if (descendants[i] == kingOrder[j]) {
                        cout << descendants[i] << " ";
                        break;
                    }
                }
            }
        }
        cout << endl << endl;
    }

    // 5. 직계 후손이 왕이 되지 못한 왕은 누구누구인가?
    void printKingsWithoutDirectSuccessors() {
        cout << "5. 직계 후손이 왕이 되지 못한 왕은 누구누구인가?" << endl;
        for (int i = 0; i < kingOrder.getSize(); i++) {
            TreeNode* king = findNode(kingOrder[i]);
            bool hasKingChild = false;

            for (int j = 0; j < king->childCount; j++) {
                // 자식이 왕인지 확인
                for (int k = 0; k < kingOrder.getSize(); k++) {
                    if (king->children[j]->name == kingOrder[k]) {
                        hasKingChild = true;
                        break;
                    }
                }
                if (hasKingChild) break;
            }

            if (!hasKingChild) {
                cout << kingOrder[i] << " ";
            }
        }
        cout << endl << endl;
    }

    // 6. 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?
    void printKingWithMostKingDescendants() {
        cout << "6. 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?" << endl;
        string maxKing;
        int maxCount = 0;

        for (int i = 0; i < kingOrder.getSize(); i++) {
            TreeNode* king = findNode(kingOrder[i]);

            // 직접적인 자식들 중에서 왕이 된 수를 계산
            int kingChildCount = 0;
            for (int j = 0; j < king->childCount; j++) {
                for (int k = 0; k < kingOrder.getSize(); k++) {
                    if (king->children[j]->name == kingOrder[k]) {
                        kingChildCount++;
                        break;
                    }
                }
            }

            if (kingChildCount > maxCount) {
                maxCount = kingChildCount;
                maxKing = kingOrder[i];
            }
        }

        cout << maxKing << " (직계 자식 왕: " << maxCount << "명)" << endl << endl;
    }

    // 7. 정종의 형제로 조선의 왕이 된 사람은 누구인가?
    void printJeongJongSiblings() {
        cout << "7. 정종의 형제로 조선의 왕이 된 사람은 누구인가?" << endl;
        TreeNode* jeongjong = findNode("정종");
        if (jeongjong != nullptr && jeongjong->parent != nullptr) {
            TreeNode* parent = jeongjong->parent;
            for (int i = 0; i < parent->childCount; i++) {
                if (parent->children[i] != jeongjong) {
                    // 형제가 왕인지 확인
                    for (int j = 0; j < kingOrder.getSize(); j++) {
                        if (parent->children[i]->name == kingOrder[j]) {
                            cout << parent->children[i]->name << " ";
                            break;
                        }
                    }
                }
            }
        }
        cout << endl << endl;
    }

    // 8. 순종의 직계 선조를 모두 출력
    void printSunjongAncestors() {
        cout << "8. 순종의 직계 선조를 모두 출력하시오:" << endl;
        TreeNode* sunjong = findNode("순종");
        if (sunjong != nullptr) {
            StringArray ancestors;
            collectAncestors(sunjong, ancestors);
            for (int i = 0; i < ancestors.getSize(); i++) {
                cout << ancestors[i];
                if (i < ancestors.getSize() - 1) cout << " -> ";
            }
        }
        cout << endl << endl;
    }

    // 9. 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가?
    void printKingsWithTwoOrMoreKingChildren() {
        cout << "9. 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가?" << endl;
        int count = 0;

        for (int i = 0; i < kingOrder.getSize(); i++) {
            TreeNode* king = findNode(kingOrder[i]);

            // 직접적인 자식들 중에서 왕이 된 수를 계산
            int kingChildCount = 0;
            for (int j = 0; j < king->childCount; j++) {
                for (int k = 0; k < kingOrder.getSize(); k++) {
                    if (king->children[j]->name == kingOrder[k]) {
                        kingChildCount++;
                        break;
                    }
                }
            }

            if (kingChildCount >= 2) {
                count++;
                cout << kingOrder[i] << " (직계 자식 왕: " << kingChildCount << "명) ";
            }
        }

        cout << endl << "총 " << count << "명" << endl << endl;
    }

    // 10. 예종은 태종의 몇 대 후손인가?
    void printYejongGenerationFromTaejong() {
        cout << "10. 예종은 태종의 몇 대 후손인가?" << endl;
        TreeNode* yejong = findNode("예종");
        TreeNode* taejong = findNode("태종");

        if (yejong != nullptr && taejong != nullptr) {
            int distance = getGenerationDistance(taejong, yejong);
            if (distance > 0) {
                cout << distance << "대 후손" << endl;
            }
            else {
                cout << "직계 관계가 아닙니다" << endl;
            }
        }
        cout << endl;
    }
};

int main() {
    JoseonDynastyTree tree;

    cout << "조선왕조.txt 파일을 읽어옵니다..." << endl << endl;
    tree.loadFromFile("C:/Users/USER/OneDrive/문서/카카오톡 받은 파일/조선왕조.txt");

    // 10가지 질문에 대한 답변
    tree.printKingsInOrder();
    tree.printKingsInReverse();
    tree.printTotalKingCount();
    tree.printInjoDescendants();
    tree.printKingsWithoutDirectSuccessors();
    tree.printKingWithMostKingDescendants();
    tree.printJeongJongSiblings();
    tree.printSunjongAncestors();
    tree.printKingsWithTwoOrMoreKingChildren();
    tree.printYejongGenerationFromTaejong();

    return 0;
}
