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

    public:
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
            value_type data;
            colourT colour;
            node *pre;
            node *next;

            node(node *left_son_, node *right_son_, node *father_,
                 value_type data_, colourT colour_ = black,
                 node *pre_ = nullptr, node *next_ = nullptr) : data(data_) {
                left_son = left_son_;
                right_son = right_son_;
                father = father_;
                colour = colour_;
                pre = pre_;
                next = next_;
            }
        };

        node *root;
        size_t siz = 0;
        node *head;
        node *tail;

        void traverse_copy(node *now_root, node *other_root, node *&min, node *&max) {
            //min_node表以now_root为根的节点中最小的节点；max_node同理
            if (other_root->left_son != nullptr) {
                now_root->left_son = new node(nullptr, nullptr, now_root,
                                              other_root->left_son->data,
                                              other_root->left_son->colour);
                traverse_copy(now_root->left_son, other_root->left_son,
                              min, now_root->pre);
                now_root->pre->next = now_root;
            } else { min = now_root; }
            if (other_root->right_son != nullptr) {
                now_root->right_son = new node(nullptr, nullptr, now_root,
                                               other_root->right_son->data,
                                               other_root->right_son->colour);
                traverse_copy(now_root->right_son, other_root->right_son,
                              now_root->next, max);
                now_root->next->pre = now_root;
            } else { max = now_root; }
        }

        void traverse_delete() {
            node *p = head, *del = head;
            while (p != nullptr) {
                p = p->next;
                delete del;
                del = p;
            }
        }

        void LL(node *root_now) {
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
            if (root_now == root) { root = root_new; }
        }

        void RR(node *root_now) {
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
            if (root_now == root) { root = root_new; }
        }

        void swap_node(node *&one, node *&two) {
            swap(one->father, two->father);
            if (one->father != nullptr) {
                if (one->father->left_son == two) { one->father->left_son = one; }
                else { one->father->right_son = one; }
            }
            if (two->father != nullptr) {
                if (two->father->left_son == one) { two->father->left_son = two; }
                else { two->father->right_son = two; }
            }
            swap(one->left_son, two->left_son);
            if (one->left_son != nullptr) { one->left_son->father = one; }
            if (two->left_son != nullptr) { two->left_son->father = two; }
            swap(one->right_son, two->right_son);
            if (one->right_son != nullptr) { one->right_son->father = one; }
            if (two->right_son != nullptr) { two->right_son->father = two; }
            swap(one->colour, two->colour);
            if (one == root) { root = two; }
            else if (two == root) { root = one; }
        }

        template<class T_swap>
        inline static void swap(T_swap &one, T_swap &two) {
            T_swap tmp = one;
            one = two;
            two = tmp;
        }

        inline void adjust_insert_link(node *now_node) {//插入节点后，前后节点、头尾节点的维护
            if (now_node->pre != nullptr) {
                now_node->pre->next = now_node;
            } else { head = now_node; }
            if (now_node->next != nullptr) {
                now_node->next->pre = now_node;
            } else { tail = now_node; }
        }

        inline void adjust_delete_link(node *now_node) {//删除节点前，前后节点、头尾节点的维护
            if (now_node->pre != nullptr) {
                now_node->pre->next = now_node->next;
            } else { head = now_node->next; }
            if (now_node->next != nullptr) {
                now_node->next->pre = now_node->pre;
            } else { tail = now_node->pre; }
        }

        inline static bool is_left_son_of_father(node *p) {
            return (p->father != nullptr && p->father->left_son == p);
        }

        inline static bool have_red_left_son(node *p) {
            return (p->left_son != nullptr && p->left_son->colour == red);
        }

        inline static bool have_null_left_son(node *p) {
            return (p->left_son == nullptr);
        }

        inline static bool have_red_right_son(node *p) {
            return (p->right_son != nullptr && p->right_son->colour == red);
        }

        inline static bool have_null_right_son(node *p) {
            return (p->right_son == nullptr);
        }


        bool check_node(node *a, colourT b = black) {
            if (a == nullptr) { return true; }
            if (a->left_son != nullptr && !Compare()(a->left_son->data.first, a->data.first)) {
                return false;
            }
            if (a->right_son != nullptr && !Compare()(a->data.first, a->right_son->data.first)) {
                return false;
            }
            if (b == red && a->colour == red) { return false; }
            return check_node(a->left_son, a->colour) && check_node(a->right_son, a->colour);
        }

        bool check_link() {
            if (head == nullptr) { return true; }
            node *p = head;
            if (p == tail) { return siz == 1; }
            int s = 1;
            while (p->next != nullptr) {
                if (!Compare()(p->data.first, p->next->data.first)) { return false; }
                p = p->next;
                ++s;
            }
            return s == siz;
        }

        bool check_road(node *p, int &h) {
            if (p == nullptr) {
                h = 0;
                return true;
            }
            int h1, h2;
            bool f1 = check_road(p->left_son, h1);
            bool f2 = check_road(p->right_son, h2);
            if (!(f1 && f2)) { return false; }
            if (h1 != h2) { return false; }
            if (p->colour == red) {
                h = h1;
                return true;
            } else {
                h = h1 + 1;
                return true;
            }
        }

        void check() {
            int h;
            std::cout << "\n\nCHECK BEGIN!\n";
            std::cout << "check the length of black road:                             ";
            if (check_road(root, h)) {
                std::cout << "PASSED with the road length<" << h << ">\n";
            } else { std::cout << "FAILED\n"; }
            std::cout << "check the colour and compare between father and son:        ";
            if (check_node(root)) { std::cout << "PASSED\n"; }
            else { std::cout << "FAILED\n"; }
            std::cout << "check the link of nodes:                                    ";
            if (check_link()) { std::cout << "PASSED\n"; }
            else { std::cout << "FAILED\n"; }
            std::cout << "CHECK END!\n\n";
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

            friend class const_iterator;

            friend class my_iterator_traits<iterator>;

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
                iter_point = iter_point->next;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }

            iterator &operator--() {
                if (map_point->siz == 0 || iter_point == map_point->head) {
                    throw invalid_iterator();
                }
                if (iter_point == nullptr) { iter_point = map_point->tail; }
                else { iter_point = iter_point->pre; }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return iter_point->data;
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

            pointer operator->() const noexcept { return &(iter_point->data); }

            map *get_map_point() const { return map_point; }

            node *get_iter_point() const { return iter_point; }

        };

        class const_iterator {

            friend class iterator;

            friend class my_iterator_traits<const_iterator>;

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
                iter_point = iter_point->next;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            const_iterator &operator--() {
                if (map_point->siz == 0 || iter_point == map_point->head) {
                    throw invalid_iterator();
                }
                if (iter_point == nullptr) { iter_point = map_point->tail; }
                else { iter_point = iter_point->pre; }
                return *this;
            }

            value_type &operator*() const {
                if (iter_point == nullptr) { throw runtime_error(); }
                return iter_point->data;
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

            const value_type *operator->() const noexcept { return &(iter_point->data); }

            const map *get_map_point() const { return map_point; }

            node *get_iter_point() const { return iter_point; }
        };

        map() {
            root = nullptr;
            siz = 0;
            head = tail = nullptr;
        }

        map(const map &other) {
            siz = other.siz;
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, nullptr, other.root->data, black);
                head = tail = root;
                traverse_copy(root, other.root, head, tail);
            } else {
                head = tail = root = nullptr;
            }
        }

        map(map &&other) {
            siz = other.siz;
            head = other.head;
            tail = other.tail;
            root = other.root;
            other.head = other.tail = other.tail = nullptr;
        }

        map &operator=(const map &other) {
            if (&other == this) { return *this; }
            traverse_delete();
            if (other.root != nullptr) {
                root = new node(nullptr, nullptr, nullptr, other.root->data, black);
                head = tail = root;
                traverse_copy(root, other.root, head, tail);
            } else {
                head = tail = root = nullptr;
            }
            siz = other.siz;
            return *this;
        }

        map &operator=(map &&other) {
            siz = other.siz;
            head = other.head;
            tail = other.tail;
            root = other.root;
            other.head = other.tail = other.tail = nullptr;
            return *this;
        }

        ~map() {
            traverse_delete();
        }

        //access specified element with bounds checking
        //Returns a reference to the mapped value of the element with key equivalent to key.
        // If no such element exists, an exception of type `index_out_of_bound'
        T &at(const Key &key) {
            node *p = root;
            if (root == nullptr) { throw index_out_of_bound(); }
            while (p != nullptr) {
                if (Compare()(key, p->data.first)) {
                    p = p->left_son;
                } else if (Compare()(p->data.first, key)) {
                    p = p->right_son;
                } else { return p->data.second; }
            }
            throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            node *p = root;
            if (root == nullptr) { throw index_out_of_bound(); }
            while (p != nullptr) {
                if (Compare()(key, p->data.first)) {
                    p = p->left_son;
                } else if (Compare()(p->data.first, key)) {
                    p = p->right_son;
                } else { return p->data.second; }
            }
            throw index_out_of_bound();
        }

        //access specified element
        // Returns a reference to the value that is mapped to a key equivalent to key,
        //performing an insertion if such key does not already exist.
        T &operator[](const Key &key) {
            return (insert(value_type(key, T())).first)->second;
        }

        //behave like at() throw index_out_of_bound if such key does not exist.
        const T &operator[](const Key &key) const { return at(key); }

        iterator begin() { return iterator(this, head); }

        const_iterator cbegin() const { return const_iterator(this, head); }

        //return a iterator to the end
        //in fact, it returns past-the-end.
        iterator end() { return iterator(this, nullptr); }

        const_iterator cend() const { return const_iterator(this, nullptr); }

        bool empty() const { return siz == 0; }

        size_t size() const { return siz; }

        void clear() {
            if (root != nullptr) { traverse_delete(); }
            siz = 0;
            head = tail = root = nullptr;
        }

        //insert an element.
        //return a pair, the first of the pair is
        //the iterator to the new element (or the element that prevented the insertion),
        //the second one is true if insert successfully, or false.
        pair<iterator, bool> insert(const value_type &value) {
            if (siz == 0) {
                head = tail = root = new node(nullptr, nullptr, nullptr, value, black);
                ++siz;
                return pair<iterator, bool>(iterator(this, root), true);
            }
            node *p = root, *p_insert;
            bool flag = false;
            while (true) {//寻找要插入的位置并插入,最后break时p为插入节点的父节点
                if (Compare()(value.first, p->data.first)) {
                    if (have_null_left_son(p)) {
                        p_insert = p->left_son = new node(nullptr, nullptr, p,
                                                          value, red, p->pre, p);
                        ++siz;
                        adjust_insert_link(p_insert);//双链表中插入节点
                        break;
                    } else { p = p->left_son; }
                } else if (Compare()(p->data.first, value.first)) {
                    if (have_null_right_son(p)) {
                        p_insert = p->right_son = new node(nullptr, nullptr, p,
                                                           value, red, p, p->next);
                        ++siz;
                        adjust_insert_link(p_insert);//双链表中插入节点
                        break;
                    } else { p = p->right_son; }
                } else { return pair<iterator, bool>(iterator(this, p), false); }
            }
            //开始向上调整
            if (p->colour == black) {
                return pair<iterator, bool>(iterator(this, p_insert), true);
            }
            while (!flag) {
                if (p->father->colour == red) { p = p->father; }//向上一层
                flag = adjust_insert(p);
                p = p->father;
            }
            return pair<iterator, bool>(iterator(this, p_insert), true);
        }

        bool adjust_insert(node *p) {
            //p表示当前节点，插入发生在p的左/右子树中，插入不会改变黑路径长度，只会产生连续红节点
            //插入节点总置为红色，调整至无连续红色返回true，否则返回false
            //连续红色发生在p和p的子节点之间
            //若p->father调整为红色，继续向上调整，返回false；反之，返回true
            if (p->father->colour == black && have_red_left_son(p->father) &&
                have_red_right_son(p->father)) {
                p->father->left_son->colour = p->father->right_son->colour = black;
                if (p->father == root) { return true; }//调整到根，结束
                else {
                    p->father->colour = red;
                    return false;//继续向上调整
                }
            }
            if (is_left_son_of_father(p)) {
                if (have_red_right_son(p)) {
                    RR(p);
                    p = p->father;
                }//需双旋，先旋转子树
                LL(p->father);//再旋
                p->right_son->colour = red;
                p->colour = black;
                return true;
            } else {
                if (have_red_left_son(p)) {
                    LL(p);
                    p = p->father;
                }//需双旋，先旋转子树
                RR(p->father);//再旋
                p->left_son->colour = red;
                p->colour = black;
                return true;
            }
        }

        //erase the element at pos.
        //throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
        void erase(iterator pos) {
            if (pos.get_map_point() != this || pos.get_iter_point() == nullptr) {
                throw invalid_iterator();
            }
            if (siz == 1) {
                delete root;
                head = tail = root = nullptr;
                --siz;
                return;
            }
            --siz;
            node *p = pos.get_iter_point();
            if (!have_null_left_son(p) && !have_null_right_son(p)) {//两儿子情况，转为一个儿子或叶节点情况
                //寻找替身，且尽量寻找红色替身以避免旋转，若前后替身均为黑色，则取后替身
                if (p->pre->colour == red) {
                    swap_node(p, p->pre);
                } else {
                    swap_node(p, p->next);
                }
            }
            adjust_delete_link(p);//维护双链表
            if (have_red_left_son(p)) {//一个儿子情况，红黑树性质保证该节点只可能为黑、且只有一个红叶儿子
                //将p的儿子染为黑色并挂在p父亲上，同时删除p
                if (p == root) {
                    root = p->left_son;
                    root->colour = black;
                    root->father = nullptr;
                } else if (is_left_son_of_father(p)) {
                    p->father->left_son = p->left_son;
                    p->left_son->father = p->father;
                    p->left_son->colour = black;
                } else {
                    p->father->right_son = p->left_son;
                    p->left_son->father = p->father;
                    p->left_son->colour = black;
                }
                delete p;
                return;//无需继续调整，结束
            } else if (have_red_right_son(p)) {
                //将p的儿子染为黑色并挂在p父亲上，同时删除p
                if (p == root) {
                    root = p->right_son;
                    root->colour = black;
                    root->father = nullptr;
                } else if (is_left_son_of_father(p)) {
                    p->father->left_son = p->right_son;
                    p->right_son->father = p->father;
                    p->right_son->colour = black;
                } else {
                    p->father->right_son = p->right_son;
                    p->right_son->father = p->father;
                    p->right_son->colour = black;
                }
                delete p;
                return;//无需继续调整，结束
            } else {//为叶节点情况
                if (p->colour == red) {
                    if (is_left_son_of_father(p)) {
                        p->father->left_son = nullptr;
                    } else { p->father->right_son = nullptr; }
                    delete p;
                    return;//直接删除，结束
                } else {
                    node *del = p;
                    bool flag = false, dir;//dir为真，表示删除发生的子树为其父节点的左子树
                    dir = is_left_son_of_father(p);
                    if (dir) { p->father->left_son = nullptr; }
                    else { p->father->right_son = nullptr; }
                    p = p->father;
                    delete del;
                    while (!flag) {
                        flag = adjust_erase(p, dir);
                        dir = is_left_son_of_father(p);
                        p = p->father;
                        if (p == nullptr) { break; }
                    }
                }
            }
        }

        bool adjust_erase(node *p, bool dir) {
            //p表示当前节点，删除发生在p的左/右子树中
            //删除发生的子树黑路径数降低了1，每次调整后以p为根的子树重新平衡
            //每次调整，保证p颜色保持不变、或染为黑色，保证p父亲所在子树的颜色仍是正确的
            //dir为true，表失衡子树为p的左子树；反之，为右子树
            //返回false，表示p为根的子树比其兄弟树的黑路径数仍少1，需继续向上调整；反之，结束调整
            //发生旋转（即p不再为根）后，总是可终止；而需继续向上调整时，p仍保持为子树根节点
            if (p->colour == red) {
                if (dir) {
                    if (have_red_right_son(p->right_son)) {
                        RR(p);//需旋转
                        p->colour = black;
                        p->father->colour = red;
                        p->father->right_son->colour = black;
                    } else if (have_red_left_son(p->right_son)) {
                        LL(p->right_son);//双旋转
                        RR(p);
                        p->father->colour = black;
                        p->father->right_son->colour = red;
                    } else {
                        p->colour = black;
                        p->right_son->colour = red;
                    }
                } else {
                    if (have_red_left_son(p->left_son)) {
                        LL(p);//需旋转
                        p->colour = black;
                        p->father->colour = red;
                        p->father->left_son->colour = black;
                    } else if (have_red_right_son(p->left_son)) {
                        RR(p->left_son);//双旋转
                        LL(p);
                        p->father->colour = black;
                        p->father->left_son->colour = red;
                    } else {
                        p->colour = black;
                        p->left_son->colour = red;
                    }
                }
                return true;//结束调整
            } else {
                if (dir) {
                    if (p->right_son->colour == red) {//需调整的树的兄节点为红
                        RR(p);
                        p->colour = red;
                        p->father->colour = black;
                        return adjust_erase(p, true);//此时，转换为p为红的情况
                    } else {//兄弟节点为黑
                        if (have_red_right_son(p->right_son)) {
                            RR(p);
                            p->father->right_son->colour = black;
                        } else if (have_red_left_son(p->right_son)) {
                            LL(p->right_son);
                            RR(p);
                            p->father->colour = black;
                        } else {
                            p->right_son->colour = red;
                            return false;//仍未平衡，继续向上调整
                        }
                    }
                    return true;
                } else {
                    if (p->left_son->colour == red) {//需调整的树的兄节点为红
                        LL(p);
                        p->colour = red;
                        p->father->colour = black;
                        return adjust_erase(p, false);//此时，转换为p为红的情况
                    } else {//兄弟节点为黑
                        if (have_red_left_son(p->left_son)) {
                            LL(p);
                            p->father->left_son->colour = black;
                        } else if (have_red_right_son(p->left_son)) {
                            RR(p->left_son);
                            LL(p);
                            p->father->colour = black;
                        } else {
                            p->left_son->colour = red;
                            return false;//仍未平衡，继续向上调整
                        }
                    }
                    return true;
                }
            }
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
                if (Compare()(key, p->data.first)) {
                    p = p->left_son;
                } else if (Compare()(p->data.first, key)) {
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
                if (Compare()(key, p->data.first)) {
                    p = p->left_son;
                } else if (Compare()(p->data.first, key)) {
                    p = p->right_son;
                } else { return iterator(this, p); }
            }
        }

        const_iterator find(const Key &key) const {
            if (root == nullptr) { return const_iterator(this, nullptr); }
            node *p = root;
            while (true) {
                if (p == nullptr) { return const_iterator(this, nullptr); }
                if (Compare()(key, p->data.first)) {
                    p = p->left_son;
                } else if (Compare()(p->data.first, key)) {
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
