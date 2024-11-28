#ifndef S21_SET_H_
#define S21_SET_H_

#include <functional>

#include "s21_rbtree.h"

namespace s21 {

template <typename Key, typename Compare = std::less<Key>>
class set {
 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using tree_type = RBTree<key_type, Compare>;
  using iterator = typename RBTree<key_type, Compare>::iterator;
  using const_iterator = typename RBTree<key_type, Compare>::const_iterator;
  using size_type = size_t;

  /// @brief Конструктор по умолчанию.
  set() : tree_(new tree_type()) {}

  /// @brief Конструктор списка инициализации.
  /// @param items Список инициализации.
  set(std::initializer_list<value_type> const &items) : set() {
    for (auto const &item : items) {
      tree_->InsertKey(item, true);
    }
  }

  /// @brief Конструктор копирования.
  /// @param other Контейнер, который копируем.
  set(set const &other) : tree_(new tree_type(*other.tree_)) {}

  /// @brief Конструктор перемещения.
  set(set &&other) noexcept : tree_(new tree_type(std::move(*other.tree_))) {}

  /// @brief Оператор присваивания копированием.
  set &operator=(const set &other) {
    if (this != &other) {
      *tree_ = *other.tree_;
    }
    return *this;
  }

  /// @brief Оператор присваивания перемещением.
  set &operator=(set &&other) noexcept {
    if (this != &other) {
      *tree_ = std::move(*other.tree_);
    }
    return *this;
  }

  /// @brief Деструктор.
  ~set() { delete tree_; }

  /// @brief Возвращает итератор на первый элемент.
  iterator begin() { return tree_->Begin(); }

  /// @brief Возвращает итератор на первый элемент.
  const_iterator begin() const { return tree_->Begin(); }

  /// @brief Возвращает итератор на элемент, следующий за последним.
  iterator end() { return tree_->End(); }

  /// @brief Возвращает итератор на элемент, следующий за последним.
  const_iterator end() const { return tree_->End(); }

  /// @brief Возвращает количество элементов в контейнере.
  size_type size() const { return tree_->Size(); }

  /// @brief Проверяет, пустой ли контейнер.
  /// @return true, если контейнер пустой, иначе false.
  bool empty() const { return tree_->Size() == 0; }

  /// @brief Максимальное количество элементов, которое может хранить контейнер.
  size_type max_size() const { return tree_->Max_Size(); }

  /// @brief Очищает контейнер.
  void clear() { tree_->Clear(); }

  /// @brief Вставка элемента в контейнер.
  /// @param value Значение, которое будет вставлено в контейнер.
  /// @return Указатель на вставленный элемент и флаг успешности вставки.
  std::pair<iterator, bool> insert(const value_type &value) {
    return tree_->InsertKey(value, true);
  }

  /// @brief Удаляет элемент из контейнера по позиции.
  void erase(iterator pos) { tree_->Erase(pos); }

  /// @brief Обменивает содержимое контейнера с другим контейнером.
  void swap(set &other) { tree_->Swap(*other.tree_); }

  /// @brief Слияние двух контейнеров.
  void merge(set &other) { tree_->Merge(*other.tree_); }

  /// @brief Поиск элемента по значению.
  /// @param key Искомое значение.
  /// @return Указатель на элемент, если он найден, иначе nullptr.
  iterator find(const key_type &key) { return tree_->Find(key); }

  /// @brief Проверяет, содержится ли элемент в контейнере.
  bool contains(const key_type &key) { return tree_->Contains(key); }

  void print() { tree_->PrintTree(); }

  bool operator==(const set &other) const {
    return tree_->operator==(*other.tree_);
  }

  bool operator!=(const set &other) const {
    return tree_->operator!=(*other.tree_);
  }

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args) {
    return tree_->Insert_Many(std::forward<Args>(args)...);
  }

 private:
  tree_type *tree_;
};

}  // namespace s21

#endif  // S21_SET_H_
