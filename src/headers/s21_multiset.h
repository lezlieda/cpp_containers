#ifndef S21_MULTISET_
#define S21_MULTISET_

#include "s21_rbtree.h"

namespace s21 {

template <typename Key, typename Compare = std::less<Key>>
class multiset {
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
  multiset() : tree_(new tree_type()) {}

  /// @brief Конструктор списка инициализации.
  /// @param items Список инициализации.
  multiset(std::initializer_list<value_type> const &items) : multiset() {
    for (auto const &item : items) {
      tree_->InsertKey(item, false);
    }
  }

  /// @brief Конструктор копирования.
  /// @param other Контейнер, который копируем.
  multiset(multiset const &other) : tree_(new tree_type(*other.tree_)) {}

  /// @brief Конструктор перемещения.
  multiset(multiset &&other) noexcept
      : tree_(new tree_type(std::move(*other.tree_))) {}

  /// @brief Оператор присваивания копированием.
  multiset &operator=(const multiset &other) {
    if (this != &other) {
      *tree_ = *other.tree_;
    }
    return *this;
  }

  /// @brief Оператор присваивания перемещением.
  multiset &operator=(multiset &&other) noexcept {
    if (this != &other) {
      *tree_ = std::move(*other.tree_);
    }
    return *this;
  }

  /// @brief Деструктор.
  ~multiset() { delete tree_; }

  /// @brief Возвращает итератор на первый элемент.
  iterator begin() { return tree_->Begin(); }

  /// @brief Возвращает итератор на первый элемент.
  const_iterator begin() const { return tree_->Begin(); }

  /// @brief Возвращает итератор на элемент, следующий за последним.
  iterator end() { return tree_->End(); }

  /// @brief Возвращает итератор на элемент, следующий за последним.
  const_iterator end() const { return tree_->End(); }

  /// @brief Проверяет, пустой ли контейнер.
  bool empty() const { return tree_->Size() == 0; }

  /// @brief Возвращает количество элементов в контейнере.
  size_type size() const { return tree_->Size(); }

  /// @brief Максимальное количество элементов, которое может содержать
  /// контейнер.
  size_type max_size() const { return tree_->Max_Size(); }

  /// @brief Очищает контейнер.
  void clear() { tree_->Clear(); }

  /// @brief Вставляет элемент в контейнер.
  /// @param value Вставляемый элемент.
  /// @return Итератор на вставленный элемент.
  iterator insert(const value_type &value) {
    return tree_->InsertKey(value, false).first;
  }

  /// @brief Удаляет элемент из контейнера по итератору.
  void erase(iterator pos) { tree_->Erase(pos); }

  /// @brief Обменивает содержимое контейнера с другим контейнером.
  void swap(multiset &other) noexcept { tree_->Swap(*other.tree_); }

  /// @brief Сливает other в текущий контейнер.
  void merge(multiset &other) { tree_->MergeMulti(*other.tree_); }

  /// @brief Возвращает количество элементов с заданным значением.
  size_type count(const key_type &key) const {
    auto low = tree_->Lower_Bound(key);
    if (low == tree_->End() || *low != key) {
      return 0;
    }
    auto up = tree_->Upper_Bound(key);
    size_type ret = 0;
    while (low != up && *low == key) {
      ++ret;
      ++low;
    }
    return ret;
  }

  /// @brief Поиск элемента по значению.
  /// @param key Искомое значение.
  /// @return Итератор на найденный элемент.
  iterator find(const key_type &key) {
    auto low = tree_->Lower_Bound(key);
    if (low == tree_->End() || *low != key) {
      return tree_->End();
    }

    return low;
  }

  /// @brief Проверяет, содержит ли контейнер элемент с заданным значением.
  bool contains(const key_type &key) const { return tree_->Contains(key); }

  /// @brief Возвращает итератор на первый элемент, который не меньше заданного.
  iterator lower_bound(const key_type &key) { return tree_->Lower_Bound(key); }

  /// @brief Возвращает итератор на первый элемент, который больше заданного.
  iterator upper_bound(const key_type &key) { return tree_->Upper_Bound(key); }

  bool operator==(multiset const &other) const {
    return *tree_ == *other.tree_;
  }

  bool operator!=(multiset const &other) const {
    return *tree_ != *other.tree_;
  }

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args) {
    return tree_->Insert_Many_Multi(std::forward<Args>(args)...);
  }

  void print() { tree_->PrintTree(); }

 private:
  tree_type *tree_;
};

}  // namespace s21

#endif  // S21_MULTISET_
