# My_STLite

## vector
实现了vector的模板类。支持T没有默认构造函数，但是不支持其没有有拷贝构造函数。
利用动态数组实现。`insert`、`pop`函数均摊时间复杂度均为`O(1)`。

接口：
```cpp
template<class T, class Compare = std::less<T>>
class vector{
    
    class iterator {
           
        iterator();

        iterator(vector *vec_, int index_);

        iterator(const iterator &it);

        iterator operator+(const int &n) const;

        iterator operator-(const int &n) const;

        int operator-(const iterator &rhs) const;

        iterator &operator+=(const int &n);

        iterator &operator-=(const int &n);
            
        iterator operator++(int);

        iterator &operator++();

        iterator operator--(int);

        iterator &operator--();
           
        T &operator*() const;

        bool operator==(const iterator &rhs) const;
        
        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const;
        
    };


    class const_iterator {

        const_iterator();

        const_iterator(const vector *vec_, int index_);
            
        const_iterator(const const_iterator &it);

        const_iterator operator+(const int &n) const;

        const_iterator operator-(const int &n) const;

        int operator-(const const_iterator &rhs) const;

        const_iterator &operator+=(const int &n);

        const_iterator &operator-=(const int &n);

        const_iterator operator++(int);

        const_iterator &operator++();

        const_iterator operator--(int);
    
        const_iterator &operator--();
        
        T operator*() const;

        bool operator==(const iterator &rhs) const;

        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const;
            
    };
    
    vector(); 

    ~vector(); 

    vector &operator=(const vector &other);

    T &at(const size_t &pos);

    const T &at(const size_t &pos);

    T &operator[](const size_t &pos);

    const T &operator[](const size_t &pos) const;

    const T &front() const;

    const T &back() const;

    iterator begin();

    const_iterator cbegin() const;

    iterator end();

    const_iterator cend() const;

    bool empty() const;

    size_t size() const;

    void clear();

    iterator insert(iterator pos, const T &value);
 
    iterator insert(const size_t &ind, const T &value);
           
    iterator erase(iterator pos);

    iterator erase(const size_t &ind);

    void pop_back();
    
};
```

## priority queue

实现了priority queue的模板类。支持T没有默认构造函数，但是不支持其没有有拷贝构造函数。
利用二项堆实现。
`push`函数平均时间复杂度为`O(1)`，最坏`O(log n)`。
`pop`、`merge`、`top`函数时间复杂度为`O(log n)`。
接口：
```cpp
template<typename T, class Compare = std::less<T>>
class priority_queue {

    priority_queue();

    priority_queue(const priority_queue &other);

    ~priority_queue();

    priority_queue &operator=(const priority_queue &other);

    const T &top() const;

    void push(const T &e);

    void pop();

    size_t size() const;

    bool empty() const;

    void merge(priority_queue &other);
    
};
```