# My_STLite

## vector
### 综述
实现了vector的模板类。支持T没有默认构造函数，但是不支持其没有拷贝构造函数。

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

### 综述

实现了priority queue的模板类。支持T没有默认构造函数，但是不支持其没有拷贝构造函数。

利用二项堆实现

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
## map
### 综述

实现了map的模板类。T需要有默认构造函数和拷贝构造函数，支持Key类没有默认构造函数，但需要有拷贝构造函数。
利用红黑树实现。
`insert`、`erase`、`find`、`[]`、`at`函数时间复杂度为`O(log n)`。
迭代器类的`begin` 、`cbegin`、`end`、`cend`、`operator++`、`operator--`函数的时间复杂度均为严格`O(1)`。

接口：
```cpp
template<class Key,class T,class Compare = std::less<Key>>
class map {

    typedef pair<const Key, T> value_type;
     
    class iterator {

        iterator(); 

        iterator(map *map_point_, value_type *iter_point_);

        iterator(const iterator &other);

        iterator operator++(int);

        iterator &operator++();

        iterator operator--(int);
        
        iterator &operator--();

        value_type &operator*() const;
        
        bool operator==(const iterator &rhs) const;

        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const; 

        pointer operator->() const noexcept; 

    };

    class const_iterator {

        const_iterator();

        const_iterator(const map *map_point_, value_type *iter_point_);

        const_iterator(const const_iterator &other);

        const_iterator(const iterator &other);

        const_iterator operator++(int);
        
        const_iterator &operator++();
        
        const_iterator operator--(int);

        const_iterator &operator--();

        value_type &operator*() const; 

        bool operator==(const iterator &rhs) const;

        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const; 

        const value_type *operator->() const noexcept; 
        
        const map *get_map_point() const; 
        
        value_type *get_iter_point() const; 
    };

    map();

    map(const map &other);
    
    map(map &&other);

    map &operator=(const map &other);
    
    map &operator=(map &&other);

    ~map();

    T &at(const Key &key); 

    const T &at(const Key &key) const;
        
    T &operator[](const Key &key); 
        
    const T &operator[](const Key &key) const; 
        
    iterator begin();
        
    const_iterator cbegin() const;

    iterator end();

    const_iterator cend() const; 
        
    bool empty() const;

    size_t size() const;

    void clear();

    pair<iterator, bool> insert(const value_type &value); 

    void erase(iterator pos);
        
    size_t count(const Key &key) const; 

    iterator find(const Key &key);

    const_iterator find(const Key &key) const;
    
};
```

### 存储结构
将含父指针的红黑树和按Compare排序的双链表向结合。每个节点存储树的两个儿子指针、父指针、节点对应的key-T对，以及双链表的前、后指针。map类中，存储红黑树的根`root`，以及双链表的头尾指针`head` 、`tail`。具体如下所示：
![](https://notes.sjtu.edu.cn/uploads/upload_61c8538bb0f3c03cbfe58d04bd15ef47.png)

代码：
```cpp
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
        };
```
### 插入
二叉查找，找到正确位置并将新节点插入、维护双链表。将新插入节点置为红色。插入不会改变黑路径长度，只可能会产生连续红节点，此时需向上调整。直至无连续红节点或调整至根，调整结束。

设插入发生在当前节点p的子树上，且插入所在子树已经调整正确。显然，此时p和插入所在子树的根节点均为红色，否则调整结束。

不妨设p是其父亲的左儿子（若为右儿子，只需对称操作即可）。
#### 情况1、p的兄弟节点为黑节点，且插入发生在p的左子树上
如下图所示，只需对p的父节点进行一次单旋转、并重新染色即可。（蓝色表示可能为不为空的黑节点或空节点）
![](https://notes.sjtu.edu.cn/uploads/upload_488ae0b67a901d8b99cf7c1eefeea771.png)
#### 情况2、p的兄弟节点为黑节点，且插入发生在p的右子树上
如下图所示，只需对p进行一次单旋转，转换至情况1。（蓝色表示可能为不为空的黑节点或空节点）
![](https://notes.sjtu.edu.cn/uploads/upload_7b040a2604502498fcd2d4a1b20569a0.png)
#### 情况3、p的兄弟节点为红节点
如下图所示，只需对重新染色，然后p向上转移2次，继续调整。（蓝色表示可能为不为空的黑节点或空节点，a和b中至少有一个红节点）
![](https://notes.sjtu.edu.cn/uploads/upload_827691ab7f9502a53d567826815f0232.png)
### 删除

删除节点、维护双链表。若需删除节点有两个儿子，则通过双链表找到相邻元素作为替身，交换两节点所有的信息（包括指针指向、颜色，以及root的指向）。因此，最后归结为删除只有一个儿子节点和删除叶节点两种情况。

若删除只有一个儿子的节点。显然，该点到空节点的黑路径长度为0，因此红黑树性质要求该节点只能为黑节点，且有一个红叶子节点。此时，只需将子节点连接掉父节点的对应位置、并染成黑色，即可结束调整。

若删除叶节点，显然红色节点时可直接结束。当删除黑节点时，到该点的黑路径长度减1，需向上调整使其重新平衡。

设删除发生在当前节点p的子树上，删除发生子树的黑路径长度减小了1。每次调整，保证p颜色保持不变、或染为黑色，因此此时以p为根的子树的颜色是正确的。每次调整后以p为根的子树重新平衡。若黑路径长度恢复或调整至根，则结束；反之，若黑路径长度减1，继续向上调整。

不妨设删除发生在p的左子树上（若为右子树，只需对称操作即可）。

#### 情况1、p为红节点
##### 情况1.1、p的右儿子的右儿子为红节点
如下图所示，只需对p进行一次单旋转、并重新染色即可。（蓝色表示可能为任意种类节点）
![](https://notes.sjtu.edu.cn/uploads/upload_f9c37d575bbff4926a21f647e298a6a0.png)
##### 情况1.2、p的右儿子的左儿子为红节点
如下图所示，需进行双旋转。先对p右儿子进行一单旋转、再对p进行旋转，并重新染色。（蓝色表示可能为不为空的黑节点或空节点，d不可能为红节点，否则并入情况1.2）
![](https://notes.sjtu.edu.cn/uploads/upload_c95fb329e37d9958fa79e9ab57ecde3e.png)
##### 情况1.3、p的右儿子的左、右儿子均不为红节点
如下图所示，直接重新染色即可。（蓝色表示可能为不为空的黑节点或空节点）
![](https://notes.sjtu.edu.cn/uploads/upload_c6e8b3b1ceaf644a37f4b94c9bb64625.png)
#### 情况2、p为黑节点，且p的右儿子为红节点
如下图所示，对p进行一次单旋转、并重新染色，转换至情况1。（蓝色表示可能为不为空的黑节点或空节点）
![](https://notes.sjtu.edu.cn/uploads/upload_b781892b4e76b0184ea4d4a895e64636.png)
#### 情况3、p为黑节点，且p的右儿子为黑节点
##### 情况3.1、p的右儿子的右儿子为红节点
如下图所示，只需对p进行一次单旋转、并重新染色即可。（蓝色表示可能为任意种类节点）
![](https://notes.sjtu.edu.cn/uploads/upload_d2fd89ec0fea8fd09be2fdbf8738a0f2.png)
##### 情况3.2、p的右儿子的左儿子为红节点
如下图所示，需进行双旋转。先对p右儿子进行一单旋转、再对p进行旋转，并重新染色。（蓝色表示可能为不为空的黑节点或空节点，d不可能为红节点，否则并入情况1.2）
![](https://notes.sjtu.edu.cn/uploads/upload_fa3c8e7909e795cd94f385946099bcc1.png)
##### 情况3.3、p的右儿子的左、右儿子均不为红节点
如下图所示，直接重新染色，继续向上调整（绿色表示任意种类节点，蓝色表示可能为不为空的黑节点或空节点）。
![](https://notes.sjtu.edu.cn/uploads/upload_06085e21cd76a607d48dc460c4e7377f.png)

### 遇到的错误
1. 开始按照书上的写法，插入时试图将路径上所有的节点改为黑色、删除时试图将路径上所有的节点改为红色，导致时间常数巨大
2. 旋转时未更新根节点
3. 删除有两个儿子的节点、找替身，只交换了相应的节点信息，迭代器出现问题
4. const_iterator类，map_point未设为const指针，导致创建const map类的const_iterator类时，会出现this指针无法传入的问题
5. 重载[]时，先查询key值是否存在，然后不存在时再重新从头开始插入，导致多找了一遍
