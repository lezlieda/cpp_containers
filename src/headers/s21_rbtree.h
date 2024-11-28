#ifndef S21_RBTREE_H_
#define S21_RBTREE_H_

#include <functional>
#include <limits>
#include <vector>

namespace s21 {

template <typename Key, typename Compare = std::less<Key>>
class RBTree {
 private:
  struct Node;
  struct Iterator;
  struct ConstIterator;

 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
  using size_type = std::size_t;

  /// @brief Конструктор по умолчанию.
  RBTree() : header_(new Node()), size_(0) {}

  /// @brief Конструктор копирования.
  /// @param other Дерево, которое копируется.
  RBTree(const RBTree& other) : header_(new Node()), size_(0) {
    CopyTree(other);
  }

  /// @brief Оператор присваивания копированием.
  /// @param other Дерево, которое копируется.
  /// @return Копия дерева.
  RBTree operator=(const RBTree& other) {
    if (this == &other) {
      return *this;
    }
    Clear();
    CopyTree(other);
    return *this;
  }

  /// @brief Конструктор перемещения.
  /// @param other Дерево, которое перемещается.
  RBTree(RBTree&& other) noexcept : RBTree() { Swap(other); }

  /// @brief Оператор присваивания перемещением.
  /// @param other Дерево, которое перемещается.
  /// @return Перемещенное дерево.
  RBTree operator=(RBTree&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    Clear();
    Swap(other);
    return *this;
  }

  /// @brief Оператор сравнения.
  /// @param other Дерево, с которым сравнивается.
  /// @return true, если деревья не равны, иначе false.
  bool operator!=(const RBTree& other) const {
    if (Size() != other.Size()) {
      return true;
    }
    for (auto it = Begin(), itOther = other.Begin(); it != End();
         ++it, ++itOther) {
      if (*it != *itOther) {
        return true;
      }
    }
    return false;
  }

  /// @brief Оператор сравнения.
  /// @param other Дерево, с которым сравнивается.
  /// @return true, если деревья равны, иначе false.
  bool operator==(const RBTree& other) const {
    if (Size() != other.Size()) {
      return false;
    }
    for (auto it = Begin(), itOther = other.Begin(); it != End();
         ++it, ++itOther) {
      if (*it != *itOther) {
        return false;
      }
    }
    return true;
  }

  /// @brief Деструктор.
  ~RBTree() {
    Clear();
    delete header_;
  }

  /// @brief Количество элементов в дереве.
  size_type Size() const noexcept { return size_; }

  /// @brief Максимально возможное количество элементов в дереве.
  size_type Max_Size() const noexcept {
    return (std::numeric_limits<size_type>::max() / 2 - sizeof(RBTree)) /
           sizeof(Node);
  }

  /// @brief Возвращает итератор на первый элемент дерева.
  iterator Begin() noexcept {
    if (size_ == 0) {
      return End();
    }
    Node* node = Minimum();
    return iterator(node);
  }

  /// @brief Возвращает константный итератор на первый элемент дерева.
  const_iterator Begin() const noexcept {
    Node* node = Minimum();
    return const_iterator(node);
  }

  /// @brief Возвращает итератор на последний элемент дерева.
  iterator End() { return iterator(header_); }

  /// @brief Возвращает константный итератор на последний элемент дерева.
  const_iterator End() const { return const_iterator(header_); }

  /// @brief Возвращает итератор на первый элемент, который не меньше заданного.
  iterator Lower_Bound(const_reference key) {
    Node* start = Root();
    iterator res = End();
    while (start != nullptr) {
      if (!lt_(start->key_, key)) {
        res = iterator(start);
        start = start->left_;
      } else {
        start = start->right_;
      }
    }

    return res;
  }

  /// @brief Возвращает итератор на первый элемент, который больше заданного.
  iterator Upper_Bound(const_reference key) {
    Node* start = Root();
    iterator res = End();
    while (start != nullptr) {
      if (lt_(key, start->key_)) {
        res = iterator(start);
        start = start->left_;

      } else {
        start = start->right_;
      }
    }
    return res;
  }

  /// @brief Поиск элемента по ключу.
  /// @param key Ключ по которому производится поиск.
  /// @return Указатель на элемент, если он найден, иначе nullptr.
  iterator Find(const key_type& key) {
    Node* node = nullptr;
    int ret = Find(key, Root(), &node);
    if (ret == 0) {
      return iterator(node);
    } else {
      return End();
    }
  }

  /// @brief Вставка элемента в дерево.
  /// @param key Ключ для вставки.
  /// @return В случае успешной вставки возвращает пару итератор на вставленный
  /// элемент и true, иначе итератор на элемент с таким ключом и false.
  std::pair<iterator, bool> InsertKey(const key_type& key, bool uniq) {
    Node* newNode = new Node(key);
    std::pair<iterator, bool> in = InsertNode(newNode, uniq);
    if (in.second == false) delete newNode;
    return in;
  }

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> Insert_Many(Args&&... args) {
    std::vector<std::pair<iterator, bool>> res;
    for (auto&& arg : {std::forward<Args>(args)...}) {
      Node* newNode = new Node(std::move(arg));
      std::pair<iterator, bool> in = InsertNode(newNode, true);
      if (in.second == false) delete newNode;
      res.push_back(in);
    }
    return res;
  }

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> Insert_Many_Multi(Args&&... args) {
    std::vector<std::pair<iterator, bool>> res;
    for (auto&& arg : {args...}) {
      Node* newNode = new Node(arg);
      std::pair<iterator, bool> in = InsertNode(newNode, false);
      if (in.second == false) delete newNode;
      res.push_back(in);
    }
    return res;
  }

  /// @brief Удаление элемента из дерева по итератору.
  void Erase(iterator pos) {
    if (pos == End()) {
      return;
    }
    delete ExtractNode(pos);
  }

  /// @brief Удаление элемента из дерева.
  /// @param key Ключ по которому производится удаление.
  void DeleteByKey(const key_type key) {
    iterator node = Find(key);
    if (node == End()) {
      return;
    }
    delete ExtractNode(node);
  }

  /// @brief Слияние двух деревьев.
  /// @param other Дерево, которое сливается с текущим.
  void Merge(RBTree& other) {
    if (this != &other) {
      iterator it = other.Begin();
      while (it != other.End()) {
        iterator res = Find(*it);
        if (res == End()) {
          iterator tmp = it++;
          Node* node = other.ExtractNode(tmp);
          InsertNode(node, true);

        } else {
          ++it;
        }
      }
    }
  }

  void MergeMulti(RBTree& other) {
    iterator it = other.Begin();
    while (other.size_ > 0) {
      Node* node = it.node_;
      ++it;
      if (node->left_ != nullptr) {
        node->left_->parent_ = node->parent_;
      }
      if (node->right_ != nullptr) {
        node->right_->parent_ = node->parent_;
      }
      if (node->parent_->left_ == node) {
        node->parent_->left_ = nullptr;
      }
      if (node->parent_->right_ == node) {
        node->parent_->right_ = nullptr;
      }
      node->InitNode();
      InsertNode(node, false);
      --other.size_;
    }
    other.header_->parent_ = nullptr;
  }

  /// @brief Обмен содержимым двух деревьев.
  void Swap(RBTree& other) {
    std::swap(header_, other.header_);
    std::swap(size_, other.size_);
    std::swap(lt_, other.lt_);
  }

  /// @brief Чёрная высота дерева.
  /// @return Возвращает чёрную высоту дерева (без учёта листьев), если дерево
  /// сбалансировано, иначе -1.
  int BlackHeight() {
    Node* node = Root();
    return Height(node);
  }

  /// @brief Содержит ли дерево элемент с заданным ключом.
  /// @param key Ключ по которому производится поиск.
  /// @return true, если элемент найден, иначе false.
  bool Contains(const key_type& key) const {
    Node* tmp = nullptr;
    return !Find(key, Root(), &tmp);
  }

  /// @brief Вывод дерева в консоль.
  void PrintTree() {
    int level = 0;
    for (auto it = Begin(); it != End(); ++it) {
      printf("it = %d; ", level);
      ++level;
      it.PrintNode();
      printf("\n");
    }
    Node* r = Root();
    printf("root key = %d; size = %ld; black height = %d\n", r->key_, Size(),
           BlackHeight());
  }

  /// @brief Очистка дерева.
  void Clear() {
    DeleteSubTree(Root());
    Root() = nullptr;
    size_ = 0;
  }

 private:
  /// @brief Корень дерева
  Node*& Root() { return header_->parent_; }

  /// @brief Корень дерева
  Node* Root() const { return header_->parent_; }

  /// @brief Копирование дерева.
  /// @param other Дерево, которое копируется.
  void CopyTree(const RBTree& other) {
    if (other.Size() == 0) {
      return;
    }
    Node* other_copy_root = CopyNodes(other.Root(), nullptr);
    Clear();
    Root() = other_copy_root;
    Root()->parent_ = header_;
    size_ = other.size_;
    lt_ = other.lt_;
  }

  Node* CopyNodes(const Node* node, Node* parent) {
    Node* copy = new Node{node->key_};
    copy->red_ = node->red_;
    try {
      if (node->left_) copy->left_ = CopyNodes(node->left_, copy);
      if (node->right_) copy->right_ = CopyNodes(node->right_, copy);
    } catch (...) {
      DeleteSubTree(copy);
      throw;
    }
    copy->parent_ = parent;
    return copy;
  }

  /// @brief Удаление поддерева.
  /// @param node Узел, от которого производится удаление.
  void DeleteSubTree(Node* node) {
    if (node == nullptr) {
      return;
    }
    DeleteSubTree(node->left_);
    DeleteSubTree(node->right_);
    delete node;
  }

  /// @brief Поиск элемента по ключу.
  /// @param key Ключ по которому производится поиск.
  /// @param node Указатель на узел, в который записывается родитель искомого
  /// элемента. Если элемент найден, то указатель на сам элемент.
  /// @return 0, если элемент найден, -1, если элемент меньше родителя, 1, если
  /// элемент больше родителя.
  int Find(const key_type& key, Node* start, Node** node) const {
    int ret = -1;
    Node* curNode = start;
    Node* parNode = header_;
    while (curNode != nullptr) {
      if (lt_(key, curNode->key_)) {
        parNode = curNode;
        curNode = curNode->left_;
        ret = -1;
      } else if (lt_(curNode->key_, key)) {
        parNode = curNode;
        curNode = curNode->right_;
        ret = 1;
      } else {
        ret = 0;
        break;
      }
    }
    *node = parNode;
    if (ret == 0) {
      *node = curNode;
    }
    return ret;
  }

  /// @brief Чёрная высота, рассчитываемая от узла.
  /// @param node Узел, от которого производится расчет.
  /// @return Количество чёрных узлов от узла до листа, если дерево
  /// сбалансировано, иначе -1.
  int Height(Node* node) {
    if (node == nullptr) {
      return 0;
    }
    int left = Height(node->left_);
    int right = Height(node->right_);
    if (left == -1 || right == -1 || left != right) {
      return -1;
    } else {
      return left + (node->red_ == false ? 1 : 0);
    }
  }

  /// @brief Обмен значениями двух узлов.
  void SwapNodes(Node* node, Node* other) {
    // меняем ссылки родителей
    if (other->parent_->left_ == other) {  // родителя other указываем на node
      other->parent_->left_ = node;
    } else {
      other->parent_->right_ = node;
    }

    if (node == Root()) {  // если node корень, меняем указатель заголовка
      Root() = other;
    } else {  // иначе по аналогии меняем ссылки родителей
      if (node->parent_->left_ == node) {
        node->parent_->left_ = other;
      } else {
        node->parent_->right_ = other;
      }
    }

    // свапаем ссылки узлов и цвета
    std::swap(node->parent_, other->parent_);
    std::swap(node->left_, other->left_);
    std::swap(node->right_, other->right_);
    std::swap(node->red_, other->red_);

    // меняем родительские ссылки у детей
    if (node->left_) node->left_->parent_ = node;
    if (node->right_) node->right_->parent_ = node;
    if (other->left_) other->left_->parent_ = other;
    if (other->right_) other->right_->parent_ = other;
  }

  /// @brief Балаансировка дерева после извлечения узла.
  /// @param node Узел, от которого производится балансировка.
  void ExtractFixup(Node* node) noexcept {
    Node* del = node;
    Node* parent = del->parent_;
    while (del != Root() &&
           del->red_ == false) {  // пока узел черный и не корень
      if (del == parent->left_) {  // если узел левый сын
        Node* brother = parent->right_;
        if (brother->red_) {  // если брат красный
          std::swap(brother->red_,
                    parent->red_);  // меняем цвета брата и родителя
          RotateLeft(parent);  // левый поворот относительно родителя
          parent = del->parent_;  // меняем родителя и брата
          brother = parent->right_;
        }
        if (brother->red_ ==
                false &&  // если брат черный и у него нет красных детей
            (brother->left_ == nullptr || brother->left_->red_ == false) &&
            (brother->right_ == nullptr || brother->right_->red_ == false)) {
          brother->red_ = true;  // перекрашиваем брата в красный
          if (parent->red_ ==
              true) {  // если родитель красный, то перекрашиваем
            parent->red_ = false;  //  его в черный
            break;  // балаансировка закончена
          }
          del = parent;  // иначе переходим на родителя
          parent = del->parent_;
        } else {  // если брат черный и у него есть красные дети
          if (brother->left_ != nullptr &&
              brother->left_->red_ &&  // если левый ребенок красный
              (brother->right_ == nullptr ||  // правого нет или он черный
               brother->right_->red_ == false)) {
            std::swap(
                brother->red_,
                brother->left_->red_);  // меняем цвета брата и левого ребенка
            RotateRight(brother);  // правый поворот относительно брата
            brother = parent->right_;  // меняем брата
          }  // если правый ребенок красный
          brother->right_->red_ = false;  // перекрашиваем его в черный
          brother->red_ = parent->red_;  // перекрашиваем брата в цвет родителя
          parent->red_ = false;  // перекрашиваем родителя в черный
          RotateLeft(parent);  // левый поворот относительно родителя
          break;  // балансировка закончена
        }
      } else {  // аналогично для правого сына
        Node* brother = parent->left_;
        if (brother->red_) {
          std::swap(brother->red_, parent->red_);
          RotateRight(parent);
          parent = del->parent_;
          brother = parent->left_;
        }
        if (brother->red_ == false &&
            (brother->left_ == nullptr || brother->left_->red_ == false) &&
            (brother->right_ == nullptr || brother->right_->red_ == false)) {
          brother->red_ = true;
          if (parent->red_ == true) {
            parent->red_ = false;
            break;
          }
          del = parent;
          parent = del->parent_;
        } else {
          if (brother->right_ != nullptr && brother->right_->red_ &&
              (brother->left_ == nullptr || brother->left_->red_ == false)) {
            std::swap(brother->red_, brother->right_->red_);
            RotateLeft(brother);
            brother = parent->left_;
          }
          brother->left_->red_ = false;
          brother->red_ = parent->red_;
          parent->red_ = false;
          RotateRight(parent);
          break;
        }
      }
    }
  }

  /// @brief Извлечение узла из дерева.
  /// @param pos Указатель на узел, который нужно извлечь.
  /// @return Извлеченный узел.
  Node* ExtractNode(iterator pos) {
    if (pos == End()) {
      return nullptr;
    }
    Node* node = pos.node_;
    // если у удаляемого узла два сына, то меняем его с преемником, т.к.
    // преемник всегда будет иметь не более одного сына и удаляем преемника
    if (node->left_ != nullptr && node->right_ != nullptr) {
      Node* change = SearchMin(node->right_);
      SwapNodes(node, change);
    }
    // если узел черный и у него один сын, то меняем его с сыном
    if (node->red_ == false &&
        ((node->left_ != nullptr && node->right_ == nullptr) ||
         (node->right_ != nullptr && node->left_ == nullptr))) {
      Node* child = node->left_ ? node->left_ : node->right_;
      SwapNodes(node, child);
    }
    // если узел красный и у него один сын, то дерево несбалансировано
    // красный узел без детей можно просто удалить

    // если узел черный и у него нет детей, то нужна балансировка
    if (node->red_ == false && node->left_ == nullptr &&
        node->right_ == nullptr) {
      ExtractFixup(node);
    }

    if (node == Root()) {
      Root() = nullptr;
    } else {
      if (node->parent_->left_ == node) {
        node->parent_->left_ = nullptr;
      } else {
        node->parent_->right_ = nullptr;
      }
    }
    --size_;
    node->InitNode();
    return node;
  }

  /// @brief Вставка узла в дерево.
  /// @param newNode Узел, который нужно вставить.
  /// @param uniq Флаг, указывающий на то, что в дереве не может быть
  /// элементов с одинаковыми ключами если true, иначе может.
  /// @return В случае успешной вставки возвращает пару итератор на вставленный
  /// элемент и true, иначе итератор на элемент с таким ключом и false.
  std::pair<iterator, bool> InsertNode(Node* newNode, bool uniq) {
    Node* parNode = nullptr;
    int ret = Find(newNode->key_, Root(), &parNode);
    while (ret == 0) {
      if (uniq) return {iterator(parNode), false};
      Node* start = parNode->right_;
      if (start) {
        parNode = nullptr;
        ret = Find(newNode->key_, start, &parNode);
      } else {
        break;
      }
    }

    if (!size_) {
      Root() = newNode;
      newNode->parent_ = header_;
    } else {
      newNode->parent_ = parNode;
      if (ret == -1) {
        parNode->left_ = newNode;

      } else {
        parNode->right_ = newNode;
      }
    }
    InsertFixup(newNode);
    ++size_;
    return {iterator(newNode), true};
  }

  /// @brief Поворот влево.
  /// @param node Узел, от которого производится поворот.
  void RotateLeft(Node* node) {
    Node* rightNode = node->right_;
    rightNode->parent_ = node->parent_;
    if (node == Root()) {
      Root() = rightNode;
    } else if (node == node->parent_->left_) {
      node->parent_->left_ = rightNode;
    } else {
      node->parent_->right_ = rightNode;
    }
    node->right_ = rightNode->left_;
    if (rightNode->left_ != nullptr) {
      rightNode->left_->parent_ = node;
    }
    node->parent_ = rightNode;
    rightNode->left_ = node;
  }

  /// @brief Поворот вправо.
  /// @param node Узел, от которого производится поворот.
  void RotateRight(Node* node) {
    Node* leftNode = node->left_;
    leftNode->parent_ = node->parent_;
    if (node == Root()) {
      Root() = leftNode;
    } else if (node == node->parent_->right_) {
      node->parent_->right_ = leftNode;
    } else {
      node->parent_->left_ = leftNode;
    }
    node->left_ = leftNode->right_;
    if (leftNode->right_ != nullptr) {
      leftNode->right_->parent_ = node;
    }
    node->parent_ = leftNode;
    leftNode->right_ = node;
  }

  /// @brief Балансировка дерева после вставки.
  /// @param node Узел, от которого производится балансировка.
  void InsertFixup(Node* node) {
    while (node != Root() && node->parent_->red_) {  // пока родитель красный
      if (node->parent_ ==
          node->parent_->parent_->left_) {  // если родитель левый сын
        Node* uncleNode = node->parent_->parent_->right_;  // uncleNode - дядя
        if (uncleNode != nullptr &&
            uncleNode->red_) {  // если дядя не нуль и красный
          node->parent_->red_ = false;          // родитель черный
          uncleNode->red_ = false;              // дядя черный
          node->parent_->parent_->red_ = true;  // дед красный
          node = node->parent_->parent_;        // переход на деда
        } else {  // если дядя нуль или черный
          if (node == node->parent_->right_) {  // если текущий узел правый
            node = node->parent_;  // переход на родителя
            RotateLeft(node);      // левый поворот
          }  // если текущий узел левый
          node->parent_->red_ = false;          // родитель черный
          node->parent_->parent_->red_ = true;  // дед красный
          RotateRight(node->parent_->parent_);  // правый поворот
        }
      } else {  // если родитель правый сын (аналогично)
        Node* uncleNode = node->parent_->parent_->left_;
        if (uncleNode != nullptr && uncleNode->red_) {
          node->parent_->red_ = false;
          uncleNode->red_ = false;
          node->parent_->parent_->red_ = true;
          node = node->parent_->parent_;
        } else {
          if (node == node->parent_->left_) {
            node = node->parent_;
            RotateRight(node);
          }
          node->parent_->red_ = false;
          node->parent_->parent_->red_ = true;
          RotateLeft(node->parent_->parent_);
        }
      }
    }
    if (node == Root()) {  // корень всегда черный
      node->red_ = false;
    }
  }

  /// @brief Минимальный элемент в дереве.
  Node* Minimum() const {
    Node* node = Root();
    while (node->left_ != nullptr) {
      node = node->left_;
    }
    return node;
  }

  /// @brief Поиск минимального элемента.
  /// @param node Узел, от которого производится поиск.
  /// @return Указатель на минимальный элемент.
  Node* SearchMin(Node* node) const noexcept {
    while (node->left_ != nullptr) {
      node = node->left_;
    }
    return node;
  }

  /// @brief Максимальный элемент в дереве.
  Node* Maximum() const {
    Node* node = Root();
    while (node->right_ != nullptr) {
      node = node->right_;
    }
    return node;
  }

  /// @brief Поиск максимального элемента.
  /// @param node Узел, от которого производится поиск.
  /// @return Указатель на максимальный элемент.
  Node* SearchMax(Node* node) const noexcept {
    while (node->right_ != nullptr) {
      node = node->right_;
    }
    return node;
  }

  struct Node {
    /// @brief Конструктор по умолчанию.
    Node()
        : parent_(nullptr),
          left_(nullptr),
          right_(nullptr),
          red_(true),
          key_(key_type{}) {}

    /// @brief Конструктор с параметром.
    /// @param key Ключ, который будет записан в узел.
    Node(const key_type& key)
        : parent_(nullptr),
          left_(nullptr),
          right_(nullptr),
          red_(true),
          key_(key) {}

    /// @brief Конструктор с параметром, перемещающий ключ.
    /// @param key Ключ, который будет записан в узел.
    Node(const key_type&& key)
        : parent_(nullptr),
          left_(nullptr),
          right_(nullptr),
          red_(true),
          key_(std::move(key)) {}

    /// @brief Приведение узла к виду по умолчанию.
    void InitNode() {
      parent_ = nullptr;
      left_ = nullptr;
      right_ = nullptr;
      red_ = true;
    }

    /// @brief Следующий узел.
    /// @return Указатель на следующий узел.
    Node* NextNode() {
      Node* node = this;
      if (node->right_ != nullptr) {
        node = node->right_;
        while (node->left_ != nullptr) {
          node = node->left_;
        }
      } else {
        while (node->parent_ != nullptr && node->parent_->right_ == node) {
          node = node->parent_;
        }
        node = node->parent_;
      }
      return node;
    }

    /// @brief Предыдущий узел.
    /// @return Указатель на предыдущий узел.
    Node* PrevNode() {
      Node* node = this;
      if (node->left_ != nullptr) {
        node = node->left_;
        while (node->right_ != nullptr) {
          node = node->right_;
        }
      } else {
        while (node->parent_ != nullptr && node->parent_->left_ == node) {
          node = node->parent_;
        }
        node = node->parent_;
      }
      return node;
    }

    Node* parent_;
    Node* left_;
    Node* right_;
    bool red_;
    key_type key_;
  };

  /// @brief Итератор.
  struct Iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = RBTree::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = RBTree::value_type*;
    using reference = RBTree::value_type&;

    /// @brief Конструктор по умолчанию запрещен.
    Iterator() = delete;

    /// @brief Конструктор с параметром.
    explicit Iterator(Node* node) : node_(node) {}

    /// @brief Указатель на значение узла.
    reference operator*() const { return node_->key_; }

    /// @brief Префиксный инкремент.
    Iterator& operator++() {
      node_ = node_->NextNode();
      return *this;
    }

    /// @brief Постфиксный инкремент.
    Iterator operator++(int) {
      Iterator tmp = *this;
      node_ = node_->NextNode();
      return tmp;
    }

    /// @brief Префиксный декремент.
    Iterator& operator--() {
      node_ = node_->PrevNode();
      return *this;
    }

    /// @brief Постфиксный декремент.
    Iterator operator--(int) {
      Iterator tmp = *this;
      node_ = node_->PrevNode();
      return tmp;
    }

    /// @brief Оператор сравнения.
    bool operator==(const Iterator& other) const {
      return node_ == other.node_;
    }

    /// @brief Оператор сравнения.
    bool operator!=(const Iterator& other) const {
      return node_ != other.node_;
    }

    /// @brief Вывод узла в консоль.
    void PrintNode() const {
      printf("key = %d; red = %d", node_->key_, node_->red_);
      if (node_->parent_ != nullptr) {
        printf("; parent = %d", node_->parent_->key_);
      }
      if (node_->left_ != nullptr) {
        printf("; left = %d", node_->left_->key_);
      }
      if (node_->right_ != nullptr) {
        printf("; right = %d", node_->right_->key_);
      }
    }

    Node* node_;
  };

  /// @brief Константный итератор.
  struct ConstIterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = RBTree::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const RBTree::value_type*;
    using reference = const RBTree::value_type&;

    ConstIterator() = delete;
    explicit ConstIterator(Node* node) : node_(node) {}
    reference operator*() const { return node_->key_; }

    ConstIterator& operator++() {
      node_ = node_->NextNode();
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator tmp = *this;
      node_ = node_->NextNode();
      return tmp;
    }

    ConstIterator& operator--() {
      node_ = node_->PrevNode();
      return *this;
    }

    ConstIterator operator--(int) {
      ConstIterator tmp = *this;
      node_ = node_->PrevNode();
      return tmp;
    }

    bool operator==(const ConstIterator& other) const {
      return node_ == other.node_;
    }

    bool operator!=(const ConstIterator& other) const {
      return node_ != other.node_;
    }

    void PrintNode() const {
      printf("key = %d; red = %d", node_->key_, node_->red_);
      if (node_->parent_ != nullptr) {
        printf("; parent = %d", node_->parent_->key_);
      }
      if (node_->left_ != nullptr) {
        printf("; left = %d", node_->left_->key_);
      }
      if (node_->right_ != nullptr) {
        printf("; right = %d", node_->right_->key_);
      }
    }

    Node* node_;
  };

  Node* header_;
  size_type size_;
  Compare lt_;
};

}  // namespace s21

#endif  // S21_RBTREE_H_
