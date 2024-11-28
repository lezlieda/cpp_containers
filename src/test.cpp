#include <gtest/gtest.h>

#include <fstream>

#include "s21_containers.h"

TEST(set, constructors) {
  s21::set<int> s1;
  s21::set<int> s2{1, 2, 3, 4, 5};
  s21::set<int> s3(s2);
  s21::set<int> s4(std::move(s3));
  s21::set<int> s5 = s4;
  s21::set<int> s6 = std::move(s5);

  std::set<int> std1;
  std::set<int> std2{1, 2, 3, 4, 5};
  std::set<int> std3(std2);
  std::set<int> std4(std::move(std3));
  std::set<int> std5 = std4;
  std::set<int> std6 = std::move(std5);

  EXPECT_EQ(s1.size(), std1.size());
  EXPECT_EQ(s2.size(), std2.size());
  EXPECT_EQ(s3.size(), std3.size());
  EXPECT_EQ(s4.size(), std4.size());
  EXPECT_EQ(s5.size(), std5.size());
  EXPECT_EQ(s6.size(), std6.size());
}

TEST(set, operators) {
  s21::set<int> s1{1, 2, 3, 4, 5};
  s21::set<int> s2{1, 2, 3, 4, 5};
  s21::set<int> s3{6, 7, 8};
  s1 = s2;
  s1 = std::move(s3);
  s3 = s2;

  std::set<int> std1{1, 2, 3, 4, 5};
  std::set<int> std2{1, 2, 3, 4, 5};
  std::set<int> std3{6, 7, 8};
  std1 = std2;
  std1 = std::move(std3);
  std3 = std2;

  EXPECT_EQ(s1.size(), std1.size());
  EXPECT_EQ(s2.size(), std2.size());
  EXPECT_EQ(s3.size(), std3.size());

  bool b12 = s1 == s2;
  bool b13 = s1 == s3;
  bool b23 = s2 == s3;

  bool std_b12 = std1 == std2;
  bool std_b13 = std1 == std3;
  bool std_b23 = std2 == std3;

  EXPECT_EQ(b12, std_b12);
  EXPECT_EQ(b13, std_b13);
  EXPECT_EQ(b23, std_b23);

  b12 = s1 != s2;
  b13 = s1 != s3;
  b23 = s2 != s3;

  std_b12 = std1 != std2;
  std_b13 = std1 != std3;
  std_b23 = std2 != std3;

  EXPECT_EQ(b12, std_b12);
  EXPECT_EQ(b13, std_b13);
  EXPECT_EQ(b23, std_b23);
}

TEST(set, test_txt) {
  std::ifstream file("test.txt");
  std::string str;
  s21::set<std::string> s21;
  std::set<std::string> std;
  const char *delimiters = " \n\t,.:;!?-()[]{}'\"";
  while (getline(file, str)) {
    char *word = strtok((char *)str.data(), delimiters);
    while (word != nullptr) {
      s21.insert(word);
      std.insert(word);
      word = strtok(nullptr, delimiters);
    }
  }
  EXPECT_EQ(s21.size(), std.size());
  auto it1 = s21.begin();
  auto it2 = std.begin();
  for (; it1 != s21.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
  s21::set<std::string> s21_1{"the", "red", "black", "tree", "fox"};
  std::set<std::string> std_1{"the", "red", "black", "tree", "fox"};

  s21_1.merge(s21);
  std_1.merge(std);
  EXPECT_EQ(s21.size(), std.size());
  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  s21_1.insert("Lorem");
  s21_1.insert("ipsum");
  s21_1.insert("dolor");
  s21_1.insert("sit");
  s21_1.insert("amet");
  std_1.insert("Lorem");
  std_1.insert("ipsum");
  std_1.insert("dolor");
  std_1.insert("sit");
  std_1.insert("amet");

  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  s21_1.erase(s21_1.find("Figure"));
  s21_1.erase(s21_1.find("right"));
  s21_1.erase(s21_1.find("left"));
  s21_1.erase(s21_1.find("color"));
  std_1.erase(std_1.find("Figure"));
  std_1.erase(std_1.find("right"));
  std_1.erase(std_1.find("left"));
  std_1.erase(std_1.find("color"));

  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  it1 = s21_1.find("the");
  it2 = std_1.find("the");
  EXPECT_EQ(*it1, *it2);
  --it1;
  --it2;
  EXPECT_EQ(*it1, *it2);
  it1--;
  it2--;
  EXPECT_EQ(*it1, *it2);

  it1 = s21_1.find("bebra");
  it2 = std_1.find("bebra");
  EXPECT_TRUE(it1 == s21_1.end());
  EXPECT_TRUE(it2 == std_1.end());

  bool b211 = s21_1.contains("root");
  bool b212 = s21_1.contains("bebra");
  EXPECT_TRUE(b211);
  EXPECT_FALSE(b212);

  s21_1.clear();
  std_1.clear();
  EXPECT_EQ(s21_1.size(), std_1.size());
  s21_1.swap(s21);
  EXPECT_FALSE(s21_1.empty());
  EXPECT_TRUE(s21.empty());
}

TEST(set, insert_many) {
  s21::set<int> s1;
  s1.insert_many(97, 58, 8, 74, 30, 81, 41, 15, 8, 3, 93, 93, 22, 32, 84, 46,
                 96, 100, 91, 72, 60, 94, 25, 46, 89, 69, 45, 51, 88, 54, 27,
                 95, 12, 13, 91, 29, 15, 29, 56, 97);
  s21::set<int> s2{97, 58, 8,  74,  30, 81, 41, 15, 8,  3,  93, 93, 22, 32,
                   84, 46, 96, 100, 91, 72, 60, 94, 25, 46, 89, 69, 45, 51,
                   88, 54, 27, 95,  12, 13, 91, 29, 15, 29, 56, 97};
  EXPECT_EQ(s1.size(), s2.size());
  EXPECT_TRUE(s1 == s2);
}

TEST(multiset, constructors) {
  s21::multiset<int> s1;
  s21::multiset<int> s2{1, 2, 3, 4, 5};
  s21::multiset<int> s3(s2);
  s21::multiset<int> s4(std::move(s3));
  s21::multiset<int> s5 = s4;
  s21::multiset<int> s6 = std::move(s5);

  std::multiset<int> std1;
  std::multiset<int> std2{1, 2, 3, 4, 5};
  std::multiset<int> std3(std2);
  std::multiset<int> std4(std::move(std3));
  std::multiset<int> std5 = std4;
  std::multiset<int> std6 = std::move(std5);

  EXPECT_EQ(s1.size(), std1.size());
  EXPECT_EQ(s2.size(), std2.size());
  EXPECT_EQ(s3.size(), std3.size());
  EXPECT_EQ(s4.size(), std4.size());
  EXPECT_EQ(s5.size(), std5.size());
  EXPECT_EQ(s6.size(), std6.size());
}

TEST(multiset, operators) {
  s21::multiset<int> s1{1, 2, 3, 4, 5};
  s21::multiset<int> s2{1, 2, 3, 4, 5};
  s21::multiset<int> s3{6, 7, 8};
  s1 = s2;
  s1 = std::move(s3);
  s3 = s2;

  std::multiset<int> std1{1, 2, 3, 4, 5};
  std::multiset<int> std2{1, 2, 3, 4, 5};
  std::multiset<int> std3{6, 7, 8};
  std1 = std2;
  std1 = std::move(std3);
  std3 = std2;

  EXPECT_EQ(s1.size(), std1.size());
  EXPECT_EQ(s2.size(), std2.size());
  EXPECT_EQ(s3.size(), std3.size());

  bool b12 = s1 == s2;
  bool b13 = s1 == s3;
  bool b23 = s2 == s3;

  bool std_b12 = std1 == std2;
  bool std_b13 = std1 == std3;
  bool std_b23 = std2 == std3;

  EXPECT_EQ(b12, std_b12);
  EXPECT_EQ(b13, std_b13);
  EXPECT_EQ(b23, std_b23);

  b12 = s1 != s2;
  b13 = s1 != s3;
  b23 = s2 != s3;

  std_b12 = std1 != std2;
  std_b13 = std1 != std3;
  std_b23 = std2 != std3;

  EXPECT_EQ(b12, std_b12);
  EXPECT_EQ(b13, std_b13);
  EXPECT_EQ(b23, std_b23);
}

TEST(multiset, test_txt) {
  std::ifstream file("test.txt");
  std::string str;
  s21::multiset<std::string> s21;
  std::multiset<std::string> std;
  const char *delimiters = " \n\t,.:;!?-()[]{}'\"";
  while (getline(file, str)) {
    char *word = strtok((char *)str.data(), delimiters);
    while (word != nullptr) {
      s21.insert(word);
      std.insert(word);
      word = strtok(nullptr, delimiters);
    }
  }
  EXPECT_EQ(s21.size(), std.size());
  auto it1 = s21.begin();
  auto it2 = std.begin();
  for (; it1 != s21.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
  EXPECT_EQ(s21.count("the"), std.count("the"));
  EXPECT_EQ(s21.count("red"), std.count("red"));
  EXPECT_EQ(s21.count("black"), std.count("black"));
  EXPECT_EQ(s21.count("tree"), std.count("tree"));
  EXPECT_EQ(s21.count("fox"), std.count("fox"));

  s21::multiset<std::string> s21_1{"the", "red", "black", "tree", "fox"};
  std::multiset<std::string> std_1{"the", "red", "black", "tree", "fox"};

  s21_1.merge(s21);
  std_1.merge(std);
  EXPECT_EQ(s21.size(), std.size());
  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  s21_1.insert("Lorem");
  s21_1.insert("ipsum");
  s21_1.insert("dolor");
  s21_1.insert("sit");
  s21_1.insert("amet");
  std_1.insert("Lorem");
  std_1.insert("ipsum");
  std_1.insert("dolor");
  std_1.insert("sit");
  std_1.insert("amet");

  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  s21_1.erase(s21_1.find("Figure"));
  s21_1.erase(s21_1.find("right"));
  s21_1.erase(s21_1.find("left"));
  s21_1.erase(s21_1.find("color"));
  std_1.erase(std_1.find("Figure"));
  std_1.erase(std_1.find("right"));
  std_1.erase(std_1.find("left"));
  std_1.erase(std_1.find("color"));

  EXPECT_EQ(s21_1.size(), std_1.size());
  it1 = s21_1.begin();
  it2 = std_1.begin();
  for (; it1 != s21_1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }

  it1 = s21_1.find("the");
  it2 = std_1.find("the");
  EXPECT_EQ(*it1, *it2);
  --it1;
  --it2;
  EXPECT_EQ(*it1, *it2);
  it1--;
  it2--;
  EXPECT_EQ(*it1, *it2);

  it1 = s21_1.find("bebra");
  it2 = std_1.find("bebra");
  EXPECT_TRUE(it1 == s21_1.end());
  EXPECT_TRUE(it2 == std_1.end());

  it1 = s21_1.lower_bound("up");
  it2 = std_1.lower_bound("up");
  EXPECT_EQ(*it1, *it2);
  it1 = s21_1.lower_bound("down");
  it2 = std_1.lower_bound("down");
  EXPECT_EQ(*it1, *it2);

  it1 = s21_1.upper_bound("up");
  it2 = std_1.upper_bound("up");
  EXPECT_EQ(*it1, *it2);
  it1 = s21_1.upper_bound("down");
  it2 = std_1.upper_bound("down");
  EXPECT_EQ(*it1, *it2);

  bool b211 = s21_1.contains("root");
  bool b212 = s21_1.contains("bebra");
  EXPECT_TRUE(b211);
  EXPECT_FALSE(b212);

  s21.insert("the");
  s21_1.clear();
  std_1.clear();
  EXPECT_EQ(s21_1.size(), std_1.size());
  s21_1.swap(s21);
  EXPECT_FALSE(s21_1.empty());
  EXPECT_TRUE(s21.empty());
}

TEST(multiset, insert_many) {
  s21::multiset<int> s1;
  s1.insert_many(97, 58, 8, 74, 30, 81, 41, 15, 8, 3, 93, 93, 22, 32, 84, 46,
                 96, 100, 91, 72, 60, 94, 25, 46, 89, 69, 45, 51, 88, 54, 27,
                 95, 12, 13, 91, 29, 15, 29, 56, 97);
  s21::multiset<int> s2{97, 58, 8,  74,  30, 81, 41, 15, 8,  3,  93, 93, 22, 32,
                        84, 46, 96, 100, 91, 72, 60, 94, 25, 46, 89, 69, 45, 51,
                        88, 54, 27, 95,  12, 13, 91, 29, 15, 29, 56, 97};
  EXPECT_EQ(s1.size(), s2.size());
  EXPECT_TRUE(s1 == s2);
}

TEST(rbtree, operators) {
  s21::RBTree<double> rb1;
  s21::RBTree<double> rb2;
  rb1.Insert_Many(1.0, 2.0, 3.0, 4.0, 5.0);
  rb2.Insert_Many(1.0, 2.0, 3.0, 4.0, 5.0);
  rb1 = rb1;
  rb2 = rb1;
  EXPECT_TRUE(rb1 == rb2);
  // rb2 = std::move(rb2);
  rb2 = std::move(rb1);
  EXPECT_FALSE(rb1 == rb2);
  EXPECT_EQ(rb1.Size(), 0);
  rb1.Insert_Many_Multi(1.0, 2.0, 3.0, 4.0, 5.0);
  rb2.Insert_Many_Multi(1.0, 2.0, 3.0, 4.0, 5.0);
  rb1.MergeMulti(rb2);
  EXPECT_EQ(rb2.Size(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
