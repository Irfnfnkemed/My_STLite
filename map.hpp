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

        class iterator;

        class const_iterator;

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
            node *father;
            value_type *data;
            colourT colour;

            node(node *left_son_ = nullptr, node *right_son_ = nullptr, node *father_ = nullptr,
                 value_type *data_ = nullptr, colourT colour_ = black) {
                left_son = left_son_;
                right_son = right_son_;
                father = father_;
                data = data_;
                colour = colour_;
            }
        };

        node *root;
        size_t siz = 0;
        iterator min;
        iterator max;

        void traverse_copy(node *now_root, node *other_root) {
            if (other_root->left_son != nullptr) {
                now_root->left_son = new node(nullptr, nullptr, now_root,
                                              new value_type(*(other_root->left_son->data)),
                                              other_root->left_son->colour);
                traverse_copy(now_root->left_son, other_root->left_son);
            }
            if (other_root->right_son != nullptr) {
                now_root->right_son = new node(nullptr, nullptr, now_root,
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

        void LL(node *root_now, bool flag) {//flag为真，表新节点着为黑色
            node *root_new = root_now->left_son;
            root_now->left_son = root_new->right_son;
            if (root_now->left_son != nullptr) { root_now->left_son->father = root_now; }
            root_new->right_son = root_now;
            root_new->father = root_now->father;
            root_now->father = root_new;
            if (root_new->father != nullptr) {
                if (root_new->father->right_son == root_now) {
                    root_new->father->right_son = root_new;
                } else { root_new->father->left_son = root_new; }
            }
            if (flag) {
                root_new->colour = black;
                root_now->colour = red;
            } else {
                root_new->colour = red;
                root_now->colour = black;
                if (root_new->left_son != nullptr) { root_new->left_son->colour = black; }
            }
            if (root_now == root) { root = root_new; }
        }

        void RR(node *root_now, bool flag) {//flag为真，表新节点着为黑色
            node *root_new = root_now->right_son;
            root_now->right_son = root_new->left_son;
            if (root_now->right_son != nullptr) { root_now->right_son->father = root_now; }
            root_new->left_son = root_now;
            root_new->father = root_now->father;
            root_now->father = root_new;
            if (root_new->father != nullptr) {
                if (root_new->father->right_son == root_now) {
                    root_new->father->right_son = root_new;
                } else { root_new->father->left_son = root_new; }
            }
            if (flag) {
                root_new->colour = black;
                root_now->colour = red;
            } else {
                root_new->colour = red;
                root_now->colour = black;
                if (root_new->right_son != nullptr) { root_new->right_son->colour = black; }
            }
            if (root_now == root) { root = root_new; }
        }

        void swap_node(node *&one, node *&two) {
            node *tmp = one->father;
            one->father = two->father;
            two->father = tmp;
            if (one->father != nullptr) {
                if (one->father->left_son == two) { one->father->left_son = one; }
                else { one->father->right_son = one; }
            }
            if (two->father != nullptr) {
                if (two->father->left_son == one) { two->father->left_son = two; }
                else { two->father->right_son = two; }
            }
            tmp = one->left_son;
            one->left_son = two->left_son;
            two->left_son = tmp;
            if (one->left_son != nullptr) { one->left_son->father = one; }
            if (two->left_son != nullptr) { two->left_son->father = two; }
            tmp = one->right_son;
            one->right_son = two->right_son;
            two->right_son = tmp;
            if (one->right_son != nullptr) { one->right_son->father = one; }
            if (two->right_son != nullptr) { two->right_son->father = two; }
            colourT tmp_c = one->colour;
            one->colour = two->colour;
            two->colour = tmp_c;
            if (one == root) { root = two; }
            else if (two == root) { root = one; }
            tmp = one;
            one = two;
            two = tmp;
        }

        inline static bool is_left_son_of_father(node *p) {
            return (p->father != nullptr && p->father->left_son == p);
        }

        inline static bool is_right_son_of_father(node *p) {
            return (p->father != nullptr && p->father->right_son == p);
        }

        inline static bool have_red_left_son(node *p) {
            return (p->left_son != nullptr && p->left_son->colour == red);
        }

        inline static bool have_black_left_son(node *p) {
            return (p->left_son == nullptr || p->left_son->colour == black);
        }

        inline static bool have_red_right_son(node *p) {
            return (p->right_son != nullptr && p->right_son->colour == red);
        }

        inline static bool have_black_right_son(node *p) {
            return (p->right_son == nullptr || p->right_son->colour == black);
        }

        inline static bool have_red_father(node *p) {
            return (p->father != nullptr && p->father->colour == red);
        }

        inline static bool have_black_father(node *p) {
            return (p->father != nullptr && p->father->colour == black);
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */

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
            node *iter_point;

        public:

            iterator() {
                map_point = nullptr;
                iter_point = nullptr;
            }

            iterator(map *map_point_, node *iter_point_) {
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
                node *p = iter_point;
                if (p->right_son != nullptr) {
                    p = p->right_son;
                    while (p->left_son != nullptr) { p = p->left_son; }
                    iter_point = p;
                } else {
                    while (is_right_son_of_father(p)) { p = p->father; }
                    if (p->father == nullptr) { iter_point = nullptr; }
                    else { iter_point = p->father; }
                }
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }

            iterator &operator--() {
                node *p = iter_point;
                if (map_point->siz == 0) { throw invalid_iterator(); }
                if (p == nullptr) {
                    iter_point = map_point->max.iter_point;
                } else if (p->left_son != nullptr) {
                    p = p->left_son;
                    while (p->right_son != nullptr) { p = p->right_son; }
                    iter_point = p;
                } else {
                    while (is_left_son_of_father(p)) { p = p->father; }
                    if (p->father == nullptr) { throw invalid_iterator(); }
                    else { iter_point = p->father; }
                }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return *(iter_point->data);
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

            pointer operator->() const noexcept { return iter_point->data; }

            map *get_map_point() const { return map_point; }

            node *get_iter_point() const { return iter_point; }

            map *get_map_point() { return map_point; }

            void set_iterator(map *map_point_, node *iter_point_) {
                map_point = map_point_;
                iter_point = iter_point_;
            }

            void set_iterator(node *iter_point_) {
                iter_point = iter_point_;
            }
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
            node *iter_point;

        public:

            const_iterator() : map_point(nullptr), iter_point(nullptr) {}

            const_iterator(const map *map_point_, node *iter_point_) :
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
                node *p = iter_point;
                if (p->right_son != nullptr) {
                    p = p->right_son;
                    while (p->left_son != nullptr) { p = p->left_son; }
                    iter_point = p;
                } else {
                    while (is_right_son_of_father(p)) { p = p->father; }
                    if (p->father == nullptr) { iter_point = nullptr; }
                    else { iter_point = p->father; }
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            const_iterator &operator--() {
                node *p = iter_point;
                if (map_point->siz == 0) { throw invalid_iterator(); }
                if (p == nullptr) {
                    iter_point = map_point->max.iter_point;
                } else if (p->left_son != nullptr) {
                    p = p->left_son;
                    while (p->right_son != nullptr) { p = p->right_son; }
                    iter_point = p;
                } else {
                    while (is_left_son_of_father(p)) { p = p->father; }
                    if (p->father == nullptr) { throw invalid_iterator(); }
                    else { iter_point = p->father; }
                }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return *(iter_point->data);
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

            const value_type *operator->() const noexcept { return iter_point->data; }

            const map *get_map_point() const { return map_point; }

            node *get_iter_point() const { return iter_point; }
        };

        map() {
            root = nullptr;
            siz = 0;
            min.set_iterator(this, nullptr);
            max.set_iterator(this, nullptr);
        }


        map(const map &other) {
            siz = other.siz;
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, nullptr, new value_type(*(other.root->data)), black);
                traverse_copy(root, other.root);
                node *p = root;
                while (p->left_son != nullptr) { p = p->left_son; }
                min.set_iterator(this, p);
                p = root;
                while (p->right_son != nullptr) { p = p->right_son; }
                max.set_iterator(this, p);
            } else {
                root = nullptr;
                min.set_iterator(this, nullptr);
                max.set_iterator(this, nullptr);
            }
        }

        map &operator=(const map &other) {
            if (&other == this) { return *this; }
            traverse_delete(root);
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, nullptr, new value_type(*(other.root->data)), black);
                traverse_copy(root, other.root);
                node *p = root;
                while (p->left_son != nullptr) { p = p->left_son; }
                min.set_iterator(p);
                p = root;
                while (p->right_son != nullptr) { p = p->right_son; }
                max.set_iterator(p);
            } else {
                root = nullptr;
                min.set_iterator(nullptr);
                max.set_iterator(nullptr);
            }
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

        iterator begin() { return min; }

        const_iterator cbegin() const { return min; }

        //return a iterator to the end
        //in fact, it returns past-the-end.
        iterator end() { return iterator(this, nullptr); }

        const_iterator cend() const { return const_iterator(this, nullptr); }

        bool empty() const { return siz == 0; }

        size_t size() const { return siz; }

        void clear() {
            if (root != nullptr) { traverse_delete(root); }
            siz = 0;
            min.set_iterator(nullptr);
            max.set_iterator(nullptr);
        }

        //insert an element.
        //return a pair, the first of the pair is
        //the iterator to the new element (or the element that prevented the insertion),
        //the second one is true if insert successfully, or false.
        pair<iterator, bool> insert(const value_type &value) {
            node *p = root;
            if (p == nullptr) {
                root = new node(nullptr, nullptr, nullptr, new value_type(value));
                ++siz;
                min.set_iterator(root);
                max.set_iterator(root);
                return pair<iterator, bool>(iterator(this, root), true);
            }
            if (have_red_left_son(p) && have_red_right_son(p)) {
                p->left_son->colour = p->right_son->colour = black;
            }
            bool flag = false;
            while (true) {
                if (Compare()(p->data->first, value.first)) {
                    if (p->right_son == nullptr) {
                        p = p->right_son = new node(nullptr, nullptr, p, new value_type(value), red);
                        flag = true;
                    } else { p = p->right_son; }
                } else if (Compare()(value.first, p->data->first)) {
                    if (p->left_son == nullptr) {
                        p = p->left_son = new node(nullptr, nullptr, p, new value_type(value), red);
                        flag = true;
                    } else { p = p->left_son; }
                } else { return pair<iterator, bool>(iterator(this, p), false); }
                if (p->colour == black && have_red_left_son(p) && have_red_right_son(p)) {
                    p->colour = red;
                    p->left_son->colour = p->right_son->colour = black;//当前节点为黑、两子节点为红，当前节点染红、子节点染黑
                }
                if (p->colour == red && have_red_father(p)) {//当前节点、父节点均为红
                    if (is_left_son_of_father(p->father)) {
                        if (is_left_son_of_father(p)) { LL(p->father->father, true); }
                        else {
                            RR(p->father, true);
                            LL(p->father, true);
                        }
                    } else {
                        if (is_right_son_of_father(p)) { RR(p->father->father, true); }
                        else {
                            LL(p->father, true);
                            RR(p->father, true);
                        }
                    }
                }
                if (flag) {
                    ++siz;
                    if (Compare()(value.first, min->first)) { --min; }
                    if (Compare()(max->first, value.first)) { ++max; }
                    return pair<iterator, bool>(iterator(this, p), true);
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
                min.set_iterator(nullptr);
                max.set_iterator(nullptr);
                return;
            }
            if (pos.get_iter_point() == min.get_iter_point()) { ++min; }
            if (pos.get_iter_point() == max.get_iter_point()) { --max; }
            node *p = root, *p_substitution = nullptr, *p_brother = nullptr;
            bool flag = true, end_flag = false;
            if (have_black_left_son(root) && have_black_right_son(root)) {
                root->colour = red;
            }
            while (true) {
                if (flag) {
                    if (Compare()(pos->first, p->data->first)) {
                        p = p->left_son;
                    } else if (Compare()(p->data->first, pos->first)) {
                        p = p->right_son;
                    } else if (p->left_son != nullptr && p->right_son != nullptr) {
                        p_substitution = p;
                        p = p->left_son;
                        flag = false;
                    } else { end_flag = true; }
                } else {
                    if (p->right_son != nullptr) {
                        p = p->right_son;
                    } else {
                        swap_node(p_substitution, p);
                        end_flag = true;
                    }
                }
                if (p != root && p->colour == black) {
                    if (p->father->colour == black) {//将父节点调为红色
                        if (p->father->left_son == p) {
                            RR(p->father, true);
                        } else {
                            LL(p->father, true);
                        }
                    }
                    if (have_black_left_son(p) && have_black_right_son(p)) {
                        if (p->father->left_son == p) { p_brother = p->father->right_son; }
                        else { p_brother = p->father->left_son; }
                        if (p_brother == nullptr || (p_brother->colour == black &&
                                                     have_black_left_son(p_brother) &&
                                                     have_black_right_son(p_brother))) {
                            p->father->colour = black;
                            p->colour = red;
                            if (p_brother != nullptr) { p_brother->colour = red; }
                        } else {
                            if (is_left_son_of_father(p)) {
                                if (have_red_right_son(p_brother)) {
                                    RR(p->father, false);
                                    p->colour = red;
                                } else {
                                    LL(p_brother, false);
                                    RR(p->father, false);
                                    p->colour = red;
                                }
                            } else {
                                if (have_red_left_son(p_brother)) {
                                    LL(p->father, false);
                                    p->colour = red;
                                } else {
                                    RR(p_brother, false);
                                    LL(p->father, false);
                                    p->colour = red;
                                }
                            }
                        }
                    }
                }
                if (end_flag) { break; }
            }
            if (p->father != nullptr) {
                if (is_left_son_of_father(p)) {
                    if (p->left_son == nullptr) {
                        p->father->left_son = p->right_son;
                        if (p->right_son != nullptr) { p->right_son->father = p->father; }
                        if (p->colour == black) { p->right_son->colour = black; }
                    } else {
                        p->father->left_son = p->left_son;
                        if (p->left_son != nullptr) { p->left_son->father = p->father; }
                        if (p->colour == black) { p->left_son->colour = black; }
                    }
                } else {
                    if (p->left_son == nullptr) {
                        p->father->right_son = p->right_son;
                        if (p->right_son != nullptr) { p->right_son->father = p->father; }
                        if (p->colour == black) { p->right_son->colour = black; }
                    } else {
                        p->father->right_son = p->left_son;
                        if (p->left_son != nullptr) { p->left_son->father = p->father; }
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
                } else { return iterator(this, p); }
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
                } else { return const_iterator(this, p); }
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
