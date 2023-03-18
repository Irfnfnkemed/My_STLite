#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:

        struct node {
            int height;
            T *data;
            node *next_brother;
            node *first_son;
            node *last_son;


            node(int height_ = -1, T *data_ = nullptr, node *next_brother_ = nullptr,
                 node *first_son_ = nullptr, node *last_son_ = nullptr) {
                height = height_;
                data = data_;
                next_brother = next_brother_;
                first_son = first_son_;
                last_son = last_son_;
            };

        };//二项树的节点

        node *root;
        int siz;

        void traverse_copy(node *now_root, node *obj_root) {
            if (obj_root->first_son == nullptr) { return; }
            node *p_ = obj_root->first_son;
            node *p = now_root->first_son = new node(p_->height, new T(*(p_->data)));
            traverse_copy(p, p_);
            while (p_->next_brother != nullptr) {
                p_ = p_->next_brother;
                p = p->next_brother = new node(p_->height, new T(*(p_->data)));
                traverse_copy(p, p_);
            }
            now_root->last_son = p;
            return;
        }

        void traverse_delete(node *now_root) {
            if (now_root->first_son == nullptr) {
                delete now_root->data;
                delete now_root;
                return;
            }
            node *del = now_root->first_son, *p = del->next_brother;
            traverse_delete(del);
            while (p != nullptr) {
                del = p;
                p = p->next_brother;
                traverse_delete(del);
            }
            if (now_root->data != nullptr) { delete now_root->data; }
            delete now_root;
            return;
        }

    public:

        priority_queue() {
            root = new node;
            siz = 0;
        }

        priority_queue(const priority_queue &other) {
            root = new node;
            traverse_copy(root, other.root);
            siz = other.siz;
        }

        ~priority_queue() {
            traverse_delete(root);
        }

        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) { return *this; }
            else {
                traverse_delete(root);
                root = new node;
                priority_queue tmp(other);
                root->first_son = tmp.root->first_son;
                tmp.root->first_son = nullptr;
                siz = other.siz;
                return *this;
            }
        }

        const T &top() const {
            if (empty()) { throw container_is_empty(); }
            else {
                node *p_top = root->first_son, *p = p_top->next_brother;
                while (p != nullptr) {
                    if (!Compare()(*(p->data), *(p_top->data))) {
                        p_top = p;
                    }
                    p = p->next_brother;
                }
                return *(p_top->data);
            }
        }

        void push(const T &e) {
            try {//新插入节点仅可能与二项树的根节点进行比较
                node *p = root->first_son;
                int height = 0;
                while (p != nullptr && Compare()(*(p->data), e) && p->height == height) {
                    p = p->next_brother;
                    ++height;
                }//当且仅当新插入节点比当前树的根节点大、可向前连续合并时，方需继续比较
            } catch (...) { return; }
            priority_queue tmp;
            tmp.root->first_son = new node(0, new T(e));
            merge(tmp);
            ++siz;
        }

        void pop() {
            if (empty()) { throw container_is_empty(); }
            else {
                node *del = root->first_son, *del_pre = nullptr;
                node *p_pre = del, *p = del->next_brother;
                while (p != nullptr) {
                    if (!Compare()(*(p->data), *(del->data))) {
                        del = p;
                        del_pre = p_pre;
                    }
                    p_pre = p;
                    p = p->next_brother;
                }
                if (del_pre != nullptr) {
                    del_pre->next_brother = del->next_brother;
                } else {
                    root->first_son = del->next_brother;
                }
                priority_queue tmp;
                tmp.root->first_son = del->first_son;
                delete del->data;
                delete del;
                merge(tmp);
                --siz;
            }
        }

        size_t size() const { return siz; }

        bool empty() const { return siz == 0; }

        void merge(priority_queue &other) {
            node *p = root->first_son, *p_ = other.root->first_son;
            node *p_pre = new node, *p_new = new node;
            node *del_pre = p_pre, *del_new = p_new;
            p_pre->next_brother = p_new;
            bool flag = true;
            while ((p != nullptr || p_ != nullptr) && flag) {
                if (p != nullptr && p_ != nullptr &&
                    p->height == p_new->height && p_->height == p_new->height) {
                    p_new = p_new->next_brother = p;
                    p_pre = p_pre->next_brother;
                    p = p->next_brother;
                } else if (p != nullptr && p->height == p_new->height) {
                    if (Compare()(*(p->data), *(p_new->data))) {
                        if (p_new->last_son == nullptr) {
                            p_new->first_son = p_new->last_son = p;
                        } else {
                            p_new->last_son = p_new->last_son->next_brother = p;
                        }
                        ++p_new->height;
                        p = p->next_brother;
                        p_new->last_son->next_brother = nullptr;
                    } else {
                        p_pre->next_brother = p;
                        if (p->last_son == nullptr) {
                            p->first_son = p->last_son = p_new;
                        } else {
                            p->last_son = p->last_son->next_brother = p_new;
                        }
                        ++p->height;
                        p_new->next_brother = nullptr;
                        p_new = p;
                        p = p->next_brother;
                    }
                } else if (p_ != nullptr && p_->height == p_new->height) {
                    if (Compare()(*(p_->data), *(p_new->data))) {
                        if (p_new->last_son == nullptr) {
                            p_new->first_son = p_new->last_son = p_;
                        } else {
                            p_new->last_son = p_new->last_son->next_brother = p_;
                        }
                        ++p_new->height;
                        p_ = p_->next_brother;
                        p_new->last_son->next_brother = nullptr;
                    } else {
                        p_pre->next_brother = p_;
                        if (p_->last_son == nullptr) {
                            p_->first_son = p_->last_son = p_new;
                        } else {
                            p_->last_son = p_->last_son->next_brother = p_new;
                        }
                        ++p_->height;
                        p_new->next_brother = nullptr;
                        p_new = p_;
                        p_ = p_->next_brother;
                    }
                } else {
                    if (p_ == nullptr || (p != nullptr && p->height < p_->height)) {
                        p_new = p_new->next_brother = p;
                        p_pre = p_pre->next_brother;
                        p = p->next_brother;
                        if (p_ == nullptr) { flag = false; }
                    } else {
                        p_new = p_new->next_brother = p_;
                        p_pre = p_pre->next_brother;
                        p_ = p_->next_brother;
                        if (p == nullptr) { flag = false; }
                    }
                }
            }
            if (flag) { p_new->next_brother = nullptr; }
            root->first_son = del_new->next_brother;
            delete del_pre;
            delete del_new;
            other.root->first_son = nullptr;
            siz += other.siz;
            other.siz = 0;
        }
    };
}

#endif