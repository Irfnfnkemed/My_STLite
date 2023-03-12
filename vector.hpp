#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */

    template<class T>
    struct my_type_traits;

    template<class T>
    struct my_iterator_traits;

    template<typename T>
    class vector {

    private:
        /**
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        T **data;//动态数组，每个元素指向vector的元素
        int len_max;//vector可容许的最大长度,1-based
        int len;//vector已使用的长度,1-based

        /**
         * you can see RandomAccessIterator at CppReference for help.
         */

    public:

        class iterator;

        class const_iterator;

        class iterator {
            friend class my_type_traits<iterator>;

            friend class my_type_traits<const_iterator>;

            friend class const_iterator;
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        private:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::random_access_iterator_tag;///////应该是随机访问寄存器？

            vector *vec;
            int index;//当前迭代器指向元素在vector中的索引,0-based

        public:

            iterator() : vec(nullptr) {}

            iterator(vector *vec_, int index_) :
                    vec(vec_), index(index_) {}

            iterator(const iterator &it) :
                    vec(it.vec), index(it.index) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
                else { return iterator(vec, index + n); }
            }

            iterator operator-(const int &n) const {
                if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
                else { return iterator(vec, index - n); }
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (vec != rhs.vec) { throw invalid_iterator(); }
                else return index - rhs.index;
            }

            iterator &operator+=(const int &n) {
                if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
                else {
                    index += n;
                    return *this;
                }
            }

            iterator &operator-=(const int &n) {
                if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
                else {
                    index -= n;
                    return *this;
                }
            }

            /**
             *iter++
             */
            iterator operator++(int) {
                if (index + 1 > vec->len) { throw runtime_error(); }
                else {
                    ++index;
                    return iterator(vec, index - 1);
                }
            }

            /**
             *++iter
             */
            iterator &operator++() {
                if (index + 1 > vec->len) { throw runtime_error(); }
                else {
                    ++index;
                    return *this;
                }
            }

            /**
             *iter--
             */
            iterator operator--(int) {
                if (index - 1 < 0) { throw runtime_error(); }
                else {
                    --index;
                    return iterator(vec, index + 1);
                }
            }

            /**
             *--iter
             */
            iterator &operator--() {
                if (index - 1 < 0) { throw runtime_error(); }
                else {
                    --index;
                    return *this;
                }
            }

            /**
             **it
             */
            T &operator*() const {
                if (index == vec->len) { throw runtime_error(); }
                return *(vec->data[index]);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return vec == rhs.vec && index == rhs.index;
            }

            bool operator==(const const_iterator &rhs) const {
                return vec == rhs.vec && index == rhs.index;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

            bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        };

        /**
         * has same function as iterator, just for a const object.
         */
        class const_iterator {

            friend class my_type_traits<iterator>;

            friend class my_type_traits<const_iterator>;

            friend class const_iterator;

        private:

            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::random_access_iterator_tag;///////应该是随机访问寄存器？

            const vector *vec;
            int index;//当前迭代器指向元素在vector中的索引,0-based

        public:
            const_iterator() : vec(nullptr) {}

            const_iterator(const vector *vec_, int index_) :
                    vec(vec_), index(index_) {}

            const_iterator(const const_iterator &it) :
                    vec(vec), index(it.index) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
                else { return iterator(vec, index + n); }
            }

            const_iterator operator-(const int &n) const {
                if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
                else { return iterator(vec, index - n); }
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const const_iterator &rhs) const {
                if (vec != rhs.vec) { throw invalid_iterator(); }
                else return index - rhs.index;
            }

            const_iterator &operator+=(const int &n) {
                if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
                else {
                    index += n;
                    return *this;
                }
            }

            const_iterator &operator-=(const int &n) {
                if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
                else {
                    index -= n;
                    return *this;
                }
            }

            /**
             *iter++
             */
            const_iterator operator++(int) {
                if (index + 1 > vec->len) { throw runtime_error(); }
                else {
                    ++index;
                    return iterator(vec, index - 1);
                }
            }

            /**
             *++iter
             */
            const_iterator &operator++() {
                if (index + 1 > vec->len) { throw runtime_error(); }
                else {
                    ++index;
                    return *this;
                }
            }

            /**
             *iter--
             */
            const_iterator operator--(int) {
                if (index - 1 < 0) { throw runtime_error(); }
                else {
                    --index;
                    return iterator(vec, index + 1);
                }
            }

            /**
             *--iter
             */
            const_iterator &operator--() {
                if (index - 1 < 0) { throw runtime_error(); }
                else {
                    --index;
                    return *this;
                }
            }

            /**
             **it
             */
            T operator*() const {
                if (index == vec->len) { throw runtime_error(); }
                return *(vec->data[index]);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return vec == rhs.vec && index == rhs.index;
            }

            bool operator==(const const_iterator &rhs) const {
                return vec == rhs.vec && index == rhs.index;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

            bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        };

        /**
         * At least two: default constructor, copy constructor
         */
        vector() {
            data = new T *[128];
            len_max = 128;
            len = 0;
        }

        vector(const vector &other) {
            len_max = other.len_max;
            len = other.len;
            data = new T *[len_max];
            for (int i = 0; i < len; ++i) { data[i] = new T(*other.data[i]); }
        }

        /**
         * Destructor
         */
        ~vector() {
            for (int i = 0; i < len; ++i) { delete data[i]; }
            delete[]data;
        }

        /**
         * Assignment operator
         */
        vector &operator=(const vector &other) {
            if (this == &other) { return *this; }
            else {
                for (int i = 0; i < len; ++i) { delete data[i]; }
                delete[]data;
                len_max = other.len_max;
                len = other.len;
                data = new T *[len_max];
                for (int i = 0; i < len; ++i) { data[i] = new T(*other.data[i]); }
                return *this;
            }
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
            else { return *data[pos]; }
        }

        const T &at(const size_t &pos) const {
            if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
            else { return *data[pos]; }
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
            else { return *data[pos]; }
        }

        const T &operator[](const size_t &pos) const {
            if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
            else { return *data[pos]; }
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (len == 0) { throw container_is_empty(); }
            else { return *data[0]; }
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (len == 0) { throw container_is_empty(); }
            else { return *data[len - 1]; }
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() { return iterator(this, 0); }

        const_iterator cbegin() const { return const_iterator(this, 0); }

        /**
         * returns an iterator to the end.
         */
        iterator end() { return iterator(this, len); }

        const_iterator cend() const { return const_iterator(this, len); }

        /**
         * checks whether the container is empty
         */
        bool empty() const { return len == 0; }

        /**
         * returns the number of elements
         */
        size_t size() const { return len; }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < len; ++i) { delete data[i]; }
            delete[]data;
            data = new T *[128];
            len_max = 128;
            len = 0;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.vec != this) { throw invalid_iterator(); }
            else if (pos.index == len) { throw runtime_error(); }
            else if (len == len_max) {
                T **tmp = new T *[len_max * 2];
                for (int i = 0; i < pos.index; ++i) {
                    tmp[i] = data[i];
                }
                tmp[pos.index] = new T(value);
                for (int i = pos.index + 1; i <= len_max; ++i) {
                    tmp[i] = data[i - 1];
                }
                delete[]data;
                data = tmp;
                len_max *= 2;
                ++len;
            } else {
                for (int i = len; i >= pos.index + 1; --i) {
                    data[i] = data[i - 1];
                }
                data[pos.index] = new T(value);
                ++len;
            }
            return iterator(this, pos.index);
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind >= len || ind < 0) { throw index_out_of_bound(); }
            else if (len == len_max) {
                T **tmp = new T *[len_max * 2];
                for (int i = 0; i < ind; ++i) {
                    tmp[i] = data[i];
                }
                tmp[ind] = new T(value);
                for (int i = ind + 1; i <= len_max; ++i) {
                    tmp[i] = data[i - 1];
                }
                delete[]data;
                data = tmp;
                len_max *= 2;
                ++len;
            } else {
                for (int i = len; i >= ind; --i) {
                    data[i] = data[i - 1];
                }
                data[ind] = new T(value);
                ++len;
            }
            return iterator(this, ind);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (pos.vec != this) { throw invalid_iterator(); }
            else if (pos.index == len) { throw runtime_error(); }
            else if (len == 0) { throw container_is_empty(); }
            else if (len - 1 >= len_max / 4 || len_max <= 128) {
                delete data[pos.index];
                for (int i = pos.index; i < len - 1; ++i) { data[i] = data[i + 1]; }
                --len;
            } else {
                T **tmp = new T *[len_max / 2];
                for (int i = 0; i < pos.index; ++i) { tmp[i] = data[i]; }
                delete data[pos.index];
                for (int i = pos.index; i < len - 1; ++i) { tmp[i] = data[i + 1]; }
                delete[]data;
                data = tmp;
                --len;
                len_max /= 2;
            }
            return iterator(this, pos.index);
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= len || ind < 0) { throw index_out_of_bound(); }
            else if (ind == len) { throw runtime_error(); }
            else if (len == 0) { throw container_is_empty(); }
            else if (len - 1 >= len_max / 4 || len_max <= 128) {
                delete data[ind];
                for (int i = ind; i < len - 1; ++i) { data[i] = data[i + 1]; }
                --len;
            } else {
                T **tmp = new T *[len_max / 2];
                for (int i = 0; i < ind; ++i) { tmp[i] = data[i]; }
                delete data[ind];
                for (int i = ind; i < len - 1; ++i) { tmp[i] = data[i + 1]; }
                delete[]data;
                data = tmp;
                --len;
                len_max /= 2;
            }
            return iterator(this, ind);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (len == len_max) {
                T **tmp = new T *[len_max * 2];
                for (int i = 0; i < len_max; ++i) {
                    tmp[i] = new T(*data[i]);
                }
                tmp[len_max] = new T(value);
                for (int i = 0; i < len_max; ++i) { delete data[i]; }
                delete[]data;
                data = tmp;
                len_max *= 2;
                ++len;
            } else {
                data[len] = new T(value);
                ++len;
            }
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (len == 0) { throw container_is_empty(); }
            if (len - 1 >= len_max / 4 || len_max <= 128) {
                delete data[len - 1];
                --len;
            } else {
                T **tmp = new T *[len_max / 2];
                for (int i = 0; i < len - 1; ++i) {
                    tmp[i] = data[i];
                }
                delete data[len - 1];
                delete[]data;
                data = tmp;
                len_max /= 2;
                --len;
            }
        }

        struct my_true_type {
        };//标识真

        struct my_false_type {
        };//标识假

        template<typename T_>
        struct my_type_traits {
            using value_type = typename T_::value_type;
            using type_assignable = my_true_type;
            using type_default_constructor = my_true_type;
            using type_copy_constructor = my_true_type;
            using type_destructor = my_true_type;
        };

        template<typename T_>
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
    };
}

#endif