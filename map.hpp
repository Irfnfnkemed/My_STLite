/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<class T>
    struct my_type_traits;

    template<class T>
    struct my_iterator_traits;

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        typedef pair<const Key, T> value_type;
    private:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        enum colourT {
            red, black
        };

        struct node {
            node *left_son;
            node *right_son;
            value_type *data;
            colourT colour;

            node(node *left_son_ = nullptr, node *right_son_ = nullptr,
                 value_type *data_ = nullptr, colourT colour_ = black) {
                left_son = left_son_;
                right_son = right_son_;
                data = data_;
                colour = colour_;
            }
        };

        node *root;
        size_t siz = 0;

        void traverse_copy(node *now_root, node *other_root) {
            if (other_root->left_son != nullptr) {
                now_root->left_son = new node(nullptr, nullptr,
                                              new value_type(*(other_root->left_son->data)),
                                              other_root->left_son->colour);
                traverse_copy(now_root->left_son, other_root->left_son);
            }
            if (other_root->right_son != nullptr) {
                now_root->right_son = new node(nullptr, nullptr,
                                               new value_type(*(other_root->right_son->data)),
                                               other_root->right_son->colour);
                traverse_copy(now_root->right_son, other_root->right_son);
            }
        }

        void traverse_delete(node *&now_root) {
            if (now_root == nullptr) { return; }
            traverse_delete(now_root->left_son);
            traverse_delete(now_root->right_son);
            delete now_root->data;
            delete now_root;
            now_root = nullptr;
        }

        node *LLb(node *root_now) {
            node *root_new = root_now->left_son;
            root_now->left_son = root_new->right_son;
            root_new->right_son = root_now;
            root_new->colour = black;
            root_now->colour = red;
            if (root_now == root) { root = root_new; }
            return root_new;
        }

        node *RRb(node *root_now) {
            node *root_new = root_now->right_son;
            root_now->right_son = root_new->left_son;
            root_new->left_son = root_now;
            root_new->colour = black;
            root_now->colour = red;
            if (root_now == root) { root = root_new; }
            return root_new;
        }

        node *LLr(node *root_now) {
            node *root_new = root_now->left_son;
            root_now->left_son = root_new->right_son;
            root_new->right_son = root_now;
            root_new->colour = red;
            root_now->colour = black;
            if (root_new->left_son != nullptr) { root_new->left_son->colour = black; }
            if (root_now == root) { root = root_new; }
            return root_new;
        }

        node *RRr(node *root_now) {
            node *root_new = root_now->right_son;
            root_now->right_son = root_new->left_son;
            root_new->left_son = root_now;
            root_new->colour = red;
            root_now->colour = black;
            if (root_new->right_son != nullptr) { root_new->right_son->colour = black; }
            if (root_now == root) { root = root_new; }
            return root_new;
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class iterator;

        class const_iterator;

        class iterator {

            friend class my_type_traits<iterator>;

            friend class my_type_traits<const_iterator>;

            friend class const_iterator;

        private:
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
            using difference_type = std::ptrdiff_t;
            using value_type = map::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;
            // If you are interested in type_traits, toy_traits_test provides a place to
            // practice. But the method used in that test is old and rarely used, so you
            // may explore on your own.
            // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
            // this part is only for bonus.
            map *map_point;
            pointer iter_point;

        public:

            iterator() {
                map_point = nullptr;
                iter_point = nullptr;
            }

            iterator(map *map_point_, value_type *iter_point_) {
                map_point = map_point_;
                iter_point = iter_point_;
            }

            iterator(const iterator &other) {
                map_point = other.map_point;
                iter_point = other.iter_point;
            }

            iterator operator++(int) {
                iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            iterator &operator++() {
                if (iter_point == nullptr) { throw invalid_iterator(); }
                node *p = map_point->root, *p_find = nullptr;
                while (p->data != iter_point) {
                    if (Compare()(iter_point->first, p->data->first)) {
                        p_find = p;
                        p = p->left_son;
                    } else if (Compare()(p->data->first, iter_point->first)) {
                        p = p->right_son;
                    }
                }
                if (p->right_son != nullptr) {
                    p = p->right_son;
                    while (p->left_son != nullptr) { p = p->left_son; }
                    iter_point = p->data;
                } else {
                    if (p_find == nullptr) { iter_point = nullptr; }
                    else { iter_point = p_find->data; }
                }
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }

            iterator &operator--() {
                node *p = map_point->root, *p_find = nullptr;
                if (p == nullptr) { throw invalid_iterator(); }
                if (iter_point == nullptr) {
                    while (p->right_son != nullptr) { p = p->right_son; }
                    iter_point = p->data;
                } else {
                    while (p->data != iter_point) {
                        if (Compare()(iter_point->first, p->data->first)) {
                            p = p->left_son;
                        } else if (Compare()(p->data->first, iter_point->first)) {
                            p_find = p;
                            p = p->right_son;
                        }
                    }
                    if (p->left_son != nullptr) {
                        p = p->left_son;
                        while (p->right_son != nullptr) { p = p->right_son; }
                        iter_point = p->data;
                    } else if (iter_point != nullptr) { iter_point = p_find->data; }
                    else { throw invalid_iterator(); }
                }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return *iter_point;
            }

            bool operator==(const iterator &rhs) const {
                return map_point == rhs.map_point && iter_point == rhs.iter_point;
            }

            bool operator==(const const_iterator &rhs) const {
                return map_point == rhs.map_point && iter_point == rhs.iter_point;
            }

            bool operator!=(const iterator &rhs) const {
                return map_point != rhs.map_point || iter_point != rhs.iter_point;
            }

            bool operator!=(const const_iterator &rhs) const {
                return map_point != rhs.map_point || iter_point != rhs.iter_point;
            }

            pointer operator->() const noexcept { return iter_point; }

            map *get_map_point() const { return map_point; }

            value_type *get_iter_point() const { return iter_point; }

        };

        class const_iterator {

            friend class my_type_traits<const_iterator>;

            friend class my_type_traits<const_iterator>;

            friend class iterator;

        private:
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            using difference_type = std::ptrdiff_t;
            using value_type = map::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;

            const map *map_point;
            value_type *iter_point;

        public:

            const_iterator() : map_point(nullptr), iter_point(nullptr) {}

            const_iterator(const map *map_point_, value_type *iter_point_) :
                    map_point(map_point_), iter_point(iter_point_) {}

            const_iterator(const const_iterator &other) :
                    map_point(other.get_map_point()), iter_point(other.get_iter_point()) {}

            const_iterator(const iterator &other) :
                    map_point(other.get_map_point()), iter_point(other.get_iter_point()) {}

            const_iterator operator++(int) {
                const_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            const_iterator &operator++() {
                if (iter_point == nullptr) { throw invalid_iterator(); }
                node *p = map_point->root, *p_find = nullptr;
                while (p->data != iter_point) {
                    if (Compare()(iter_point->first, p->data->first)) {
                        p_find = p;
                        p = p->left_son;
                    } else if (Compare()(p->data->first, iter_point->first)) {
                        p = p->right_son;
                    }
                }
                if (p->right_son != nullptr) {
                    p = p->right_son;
                    while (p->left_son != nullptr) { p = p->left_son; }
                    iter_point = p->data;
                } else {
                    if (p_find == nullptr) { iter_point = nullptr; }
                    else { iter_point = p_find->data; }
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            const_iterator &operator--() {
                node *p = map_point->root, *p_find = nullptr;
                if (iter_point == nullptr) {
                    while (p->right_son != nullptr) { p = p->right_son; }
                    iter_point = p->data;
                } else {
                    while (p->data != iter_point) {
                        if (Compare()(iter_point->first, p->data->first)) {
                            p = p->left_son;
                        } else if (Compare()(p->data->first, iter_point->first)) {
                            p_find = p;
                            p = p->right_son;
                        }
                    }
                    if (p->left_son != nullptr) {
                        p = p->left_son;
                        while (p->right_son != nullptr) { p = p->right_son; }
                        iter_point = p->data;
                    } else if (iter_point != nullptr) { iter_point = p_find->data; }
                    else { throw invalid_iterator(); }
                }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return *iter_point;
            }

            bool operator==(const iterator &rhs) const {
                return map_point == rhs.get_map_point() && iter_point == rhs.get_iter_point();
            }

            bool operator==(const const_iterator &rhs) const {
                return map_point == rhs.map_point && iter_point == rhs.iter_point;
            }

            bool operator!=(const iterator &rhs) const {
                return map_point != rhs.get_map_point() || iter_point != rhs.get_iter_point();
            }

            bool operator!=(const const_iterator &rhs) const {
                return map_point != rhs.map_point || iter_point != rhs.iter_point;
            }

            const value_type *operator->() const noexcept { return iter_point; }

            const map *get_map_point() const { return map_point; }

            value_type *get_iter_point() const { return iter_point; }

        };

        map() {
            root = nullptr;
            siz = 0;
        }


        map(const map &other) {
            siz = other.siz;
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, new value_type(*(other.root->data)), black);
                traverse_copy(root, other.root);
            } else { root = nullptr; }
        }

        map &operator=(const map &other) {
            if (&other == this) { return *this; }
            traverse_delete(root);
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, new value_type(*(other.root->data)), black);
                traverse_copy(root, other.root);
            } else { root = nullptr; }
            siz = other.siz;
            return *this;
        }

        ~map() { traverse_delete(root); }

        //access specified element with bounds checking
        //Returns a reference to the mapped value of the element with key equivalent to key.
        // If no such element exists, an exception of type `index_out_of_bound'
        T &at(const Key &key) {
            node *p = root;
            if (root == nullptr) { throw index_out_of_bound(); }
            while (p != nullptr) {
                if (Compare()(key, p->data->first)) {
                    p = p->left_son;
                } else if (Compare()(p->data->first, key)) {
                    p = p->right_son;
                } else { return p->data->second; }
            }
            throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            node *p = root;
            if (root == nullptr) { throw index_out_of_bound(); }
            while (p != nullptr) {
                if (Compare()(key, p->data->first)) {
                    p = p->left_son;
                } else if (Compare()(p->data->first, key)) {
                    p = p->right_son;
                } else { return p->data->second; }
            }
            throw index_out_of_bound();
        }

        //access specified element
        // Returns a reference to the value that is mapped to a key equivalent to key,
        //performing an insertion if such key does not already exist.
        T &operator[](const Key &key) {
            try {
                return at(key);
            } catch (...) {
                return (insert(value_type(key, T())).first)->second;
            }
        }

        //behave like at() throw index_out_of_bound if such key does not exist.
        const T &operator[](const Key &key) const { return at(key); }

        iterator begin() {
            if (root == nullptr) { return iterator(this, nullptr); }
            else {
                node *p = root;
                while (p->left_son != nullptr) { p = p->left_son; }
                return iterator(this, p->data);
            }
        }

        const_iterator cbegin() const {
            if (root == nullptr) { throw runtime_error(); }
            else {
                node *p = root;
                while (p->left_son != nullptr) { p = p->left_son; }
                return const_iterator(this, p->data);
            }
        }

        //return a iterator to the end
        //in fact, it returns past-the-end.
        iterator end() { return iterator(this, nullptr); }

        const_iterator cend() const { return const_iterator(this, nullptr); }

        bool empty() const { return siz == 0; }

        size_t size() const { return siz; }

        void clear() {
            if (root != nullptr) { traverse_delete(root); }
            siz = 0;
        }

        //insert an element.
        //return a pair, the first of the pair is
        //the iterator to the new element (or the element that prevented the insertion),
        //the second one is true if insert successfully, or false.
        pair<iterator, bool> insert(const value_type &value) {
            node *p = root, *p_father = nullptr,
                    *p_grandfather = nullptr, *p_great_grandfather = nullptr;
            if (p == nullptr) {
                root = new node(nullptr, nullptr, new value_type(value));
                ++siz;
                return pair<iterator, bool>(iterator(this, root->data), true);
            }
            if ((p->left_son != nullptr && p->left_son->colour == red) &&
                (p->right_son != nullptr && p->right_son->colour == red)) {
                p->left_son->colour = p->right_son->colour = black;
            }
            bool flag = false;
            while (true) {
                p_great_grandfather = p_grandfather;
                p_grandfather = p_father;
                p_father = p;
                if (Compare()(p->data->first, value.first)) {
                    p = p->right_son;
                    if (p == nullptr) {
                        p = p_father->right_son = new node(nullptr, nullptr, new value_type(value), red);
                        flag = true;
                    }
                } else if (Compare()(value.first, p->data->first)) {
                    p = p->left_son;
                    if (p == nullptr) {
                        p = p_father->left_son = new node(nullptr, nullptr, new value_type(value), red);
                        flag = true;
                    }
                } else { return pair<iterator, bool>(iterator(this, p->data), false); }
                if (p->colour == black &&
                    (p->left_son != nullptr && p->left_son->colour == red) &&
                    (p->right_son != nullptr && p->right_son->colour == red)) {
                    p->colour = red;
                    p->left_son->colour = p->right_son->colour = black;//当前节点为黑、两子节点为红，当前节点染红、子节点染黑
                }
                if (p->colour == red && p_father != nullptr && p_father->colour == red) {//当前节点、父节点均为红
                    node *tmp;
                    if (p_grandfather->left_son == p_father && p_father->left_son == p) {
                        tmp = LLb(p_grandfather);
                    } else if (p_grandfather->right_son == p_father &&
                               p_father->right_son == p) {
                        tmp = RRb(p_grandfather);
                    } else if (p_grandfather->left_son == p_father &&
                               p_father->right_son == p) {
                        tmp = RRb(p_father);
                        p_grandfather->left_son = tmp;
                        tmp = LLb(p_grandfather);
                        p_father = p_great_grandfather;
                    } else if (p_grandfather->right_son == p_father &&
                               p_father->left_son == p) {
                        tmp = LLb(p_father);
                        p_grandfather->right_son = tmp;
                        tmp = RRb(p_grandfather);
                        p_father = p_great_grandfather;
                    }
                    if (p_great_grandfather != nullptr) {
                        if (p_great_grandfather->left_son == p_grandfather) {
                            p_great_grandfather->left_son = tmp;
                        } else { p_great_grandfather->right_son = tmp; }
                    }
                    p_grandfather = p_father;
                }
                if (flag) {
                    ++siz;
                    return pair<iterator, bool>(iterator(this, p->data), true);
                }
            }
        }

        //erase the element at pos.
        //throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
        void erase(iterator pos) {
            if (pos.get_map_point() != this || pos.get_iter_point() == nullptr) {
                throw invalid_iterator();
            }
            if (siz == 1) {
                delete root->data;
                delete root;
                root = nullptr;
                --siz;
                return;
            }
            node *p = root, *p_father = nullptr, *p_grandfather = nullptr,
                    *p_substitution = nullptr, *p_substitution_father = nullptr,
                    *p_brother = nullptr, *tmp = nullptr;
            bool flag = true, end_flag = false;
            if ((root->left_son == nullptr || root->left_son->colour == black) &&
                (root->right_son == nullptr || root->right_son->colour == black)) {
                root->colour = red;
            }
            while (true) {
                if (flag) {
                    if (Compare()(pos->first, p->data->first)) {
                        p_grandfather = p_father;
                        p_father = p;
                        p = p->left_son;
                    } else if (Compare()(p->data->first, pos->first)) {
                        p_grandfather = p_father;
                        p_father = p;
                        p = p->right_son;
                    } else if (p->left_son != nullptr && p->right_son != nullptr) {
                        p_grandfather = p_father;
                        p_substitution_father = p_father;
                        p_father = p;
                        p_substitution = p;
                        p = p->left_son;
                        flag = false;
                    } else { end_flag = true; }
                } else {
                    if (p->right_son != nullptr) {
                        p_grandfather = p_father;
                        p_father = p;
                        p = p->right_son;
                    } else {
                        value_type *del = p_substitution->data;
                        p_substitution->data = p->data;
                        p->data = del;
                        end_flag = true;
                    }
                }
                if (p != root && p->colour == black) {
                    if (p_father->colour == black) {//将父节点调为红色
                        if (p_father->left_son == p) {
                            tmp = RRr(p_father);
                            p_father->colour = red;
                            tmp->colour = black;
                        } else {
                            tmp = LLr(p_father);
                            p_father->colour = red;
                            tmp->colour = black;
                        }
                        if (p_grandfather != nullptr) {
                            if (p_grandfather->left_son == p_father) {
                                p_grandfather->left_son = tmp;
                            } else { p_grandfather->right_son = tmp; }
                        }
                        p_grandfather = tmp;
                    }
                    if ((p->left_son == nullptr || p->left_son->colour == black) &&
                        (p->right_son == nullptr || p->right_son->colour == black)) {
                        if (p_father->left_son == p) { p_brother = p_father->right_son; }
                        else { p_brother = p_father->left_son; }
                        if (p_brother == nullptr || (p_brother->colour == black &&
                                                     (p_brother->left_son == nullptr ||
                                                      p_brother->left_son->colour == black) &&
                                                     (p_brother->right_son == nullptr ||
                                                      p_brother->right_son->colour == black))) {
                            p_father->colour = black;
                            p->colour = red;
                            if (p_brother != nullptr) { p_brother->colour = red; }
                        } else {
                            if (p_father->right_son == p_brother &&
                                p_brother->right_son != nullptr &&
                                p_brother->right_son->colour == red) {
                                tmp = RRr(p_father);
                                p->colour = red;
                            } else if (p_father->left_son == p_brother &&
                                       p_brother->left_son != nullptr &&
                                       p_brother->left_son->colour == red) {
                                tmp = LLr(p_father);
                                p->colour = red;
                            } else if (p_father->right_son == p_brother &&
                                       p_brother->left_son != nullptr &&
                                       p_brother->left_son->colour == red) {
                                tmp = LLr(p_brother);
                                p_father->right_son = tmp;
                                tmp = RRr(p_father);
                                p->colour = red;
                            } else {
                                tmp = RRr(p_brother);
                                p_father->left_son = tmp;
                                tmp = LLr(p_father);
                                p->colour = red;
                            }
                            if (p_grandfather != nullptr) {
                                if (p_grandfather->left_son == p_father) {
                                    p_grandfather->left_son = tmp;
                                } else { p_grandfather->right_son = tmp; }
                            }
                        }
                    }
                }
                if (end_flag) { break; }
            }
            if (p_father != nullptr) {
                if (p_father->left_son == p) {
                    if (p->left_son == nullptr) {
                        p_father->left_son = p->right_son;
                        if (p->colour == black) { p->right_son->colour = black; }
                    } else {
                        p_father->left_son = p->left_son;
                        if (p->colour == black) { p->left_son->colour = black; }
                    }
                } else {
                    if (p->left_son == nullptr) {
                        p_father->right_son = p->right_son;
                        if (p->colour == black) { p->right_son->colour = black; }
                    } else {
                        p_father->right_son = p->left_son;
                        if (p->colour == black) { p->left_son->colour = black; }
                    }
                }
            } else {
                if (p->left_son != nullptr) { root = p->left_son; }
                else { root = p->right_son; }
            }
            if (p->data != nullptr) { delete p->data; }
            delete p;
            --siz;
            root->colour = black;
            return;
        }

        // Returns the number of elements with key
        //  that compares equivalent to the specified argument,
        //  which is either 1 or 0
        //     since this container does not allow duplicates.
        // The default method of check the equivalence is !(a < b || b > a)
        size_t count(const Key &key) const {
            if (root == nullptr) { return 0; }
            node *p = root;
            while (true) {
                if (p == nullptr) { return 0; }
                if (Compare()(key, p->data->first)) {
                    p = p->left_son;
                } else if (Compare()(p->data->first, key)) {
                    p = p->right_son;
                } else { return 1; }
            }
        }

        //Finds an element with key equivalent to key.
        //key value of the element to search for.
        //Iterator to an element with key equivalent to key.
        //  If no such element is found, past-the-end (see end()) iterator is returned.
        iterator find(const Key &key) {
            if (root == nullptr) { return iterator(this, nullptr); }
            node *p = root;
            while (true) {
                if (p == nullptr) { return iterator(this, nullptr); }
                if (Compare()(key, p->data->first)) {
                    p = p->left_son;
                } else if (Compare()(p->data->first, key)) {
                    p = p->right_son;
                } else { return iterator(this, p->data); }
            }
        }

        const_iterator find(const Key &key) const {
            if (root == nullptr) { return const_iterator(this, nullptr); }
            node *p = root;
            while (true) {
                if (p == nullptr) { return const_iterator(this, nullptr); }
                if (Compare()(key, p->data->first)) {
                    p = p->left_son;
                } else if (Compare()(p->data->first, key)) {
                    p = p->right_son;
                } else { return const_iterator(this, p->data); }
            }
        }
    };

    struct my_true_type {
    };//标识真

    struct my_false_type {
    };//标识假

    template<typename T>
    struct my_type_traits {
        using value_type = typename T::value_type;
        using type_assignable = my_true_type;
        using type_default_constructor = my_true_type;
        using type_copy_constructor = my_true_type;
        using type_destructor = my_true_type;
    };

    template<typename T>
    struct my_iterator_traits {
        using value_type = typename T::value_type;
        using difference_type = typename T::difference_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
        using iterator_assignable = my_true_type;
        using iterator_default_constructor = my_true_type;
        using iterator_copy_constructor = my_true_type;
        using iterator_destructor = my_true_type;
    };
}

#endif
