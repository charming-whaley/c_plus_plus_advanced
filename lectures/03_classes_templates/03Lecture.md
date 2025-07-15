# Лекция 3. Шаблоны классов

## 3.1 Частичный порядок

<p>Шаблон может выиграть перегрузку</p>

```c++
// Пример 1 - более очевидный
void foo(double x);

template<typename T>
void foo(T x);

foo(1); // <- это вариант с шаблоном, а не double

// Пример 2 - более нетривиальный
template<typename T> 
void f(T);

template<typename T> 
void f(T*);

int ***a;
foo(a); // <- здесь выберется вариант с указателем, то есть, с параметром T*
```

<p>Во втором примере логика немного другая: и специализация, и инстанцирование, и вывод типов работают одновременно.</p>

## 3.2 Имена в шаблонах

<p>Рассмотрим следующий код:</p>

```c++
int y = 2; { int y = y; }
int x = 2; { int x[x]; }
```

<p>В данном примере первая строка является <b>UB</b>, так как так называемся точка определения объекта с именем <b>y</b> начинается в внутреннем scope, а поэтому не понятно как мы можем дать <b>y</b> значение <b>y</b>.</p>

<p>Точка определения (PoD) - это граница, в которой завершено объявление. До PoD имя не считается введённым в область видимости.</p>

```c++
template<typename T>
struct fwnode /* PoD */ {
    T data_;
    fwnode *next_;
};

template<typename T> 
class Stack {
    fwnode<T> *top_;
};
```

<p>Таким образом, например, мы можем использовать имя нашего объекта (допустим структуры) внутри тела структуры.</p>

<p>Рассмотрим следующий код:</p>

```c++
#include <iostream>

template<typename T> struct D {
    enum { max = 10 };
};

char buffer[D<void>::max];
extern void clear(char* buf);

int main(int argc, const char* argv[]) {
    std::cout << "Requesting to clean " << D<void>::max << std::endl;
    clear(buffer);
}
```

<p>Данный код является корректным, однако опасным. Все дело в том, что тип <b>void</b> в данном случае не совсем уместен, так как <b>void</b> - неполный тип.</p>

## 3.3 Частичная специализация

<p>Рассмотрим следующий класс <b>Stack</b>:</p>

```c++
template<typename T>
struct fwnode /* PoD */ {
    T data_;
    fwnode *next_;
};

template<typename T>
class Stack {
    fwnode<T> *top_;
};
```

<p>Для удобства тут было бы проще хранить массив указателей:</p>

```c++
// Опустим стурктуру fwnode

template<typename T>
class Stack<T*> {
    T** content_;
};
```

<p>Заметим, что такое упрощение имен работает в частичных специализациях.</p>

```c++
template<typename T>
class A {
    A* a1; // <- здесь A означает A<T>
};

template<typename T>
class A<T*> {
    A* a2; // <- здесь A означает A<T*>
};
```

<p>Для практики рассмотрим следующие примеры частичной специализации:</p>

```c++
template<typename T, typename U> class Foo {  };            // Пусть это 1
template<typename T> class Foo<T, T> {  };                  // Пусть это 2
template<typename T> class Foo<T, int> {  };                // Пусть это 3
template<typename T, typename U> class Foo<T*, U*> {  };    // Пусть это 4

Foo<int, float> mif;        // <- это будет 1
Foo<float, float> mif;      // <- это будет 2
Foo<float, int> mif;        // <- это будет 3
Foo<int*, float*> mif;      // <- это будет 4
```

<p>Стоит также учитывать очень важный момент: частично специализированный шаблон должен быть действительно менее общим, чем тот, версией которого он является:</p>

```c++
template<typename T> class X {  };
template<typename U> class X<U> {  }; // <- это уже ошибка, так как ничем не отличается от верхней
```

## 3.4 Трюк Саттера

<p>Для начала запомним главное правило: частичная специализация для функций <b>невозможна</b>.</p>

```c++
template<typename T> void foo(T x);
template<> void foo<int>(int x); // <- здесь все нормально
template<typename T> void foo<int*>(T* x); // <- а вот тут уже ошибка
```

<p>И отсюда возникает вопрос: есть ли вариант обойти такое ограничение? И на самом деле - да, оно есть. Это решение было предложено Гербом Саттером, называется такое решение <b>Трюком Саттера</b>. Суть заключается в делегировании реализации шаблонной функции к вспомогательной структуре, котороую можно специализировать по-разному в зависимости от типа:</p>

```c++
#include <iostream>

template<typename T>
struct FImpl {
    static int f(T t) { 
        return 0; 
    }
};

// Частично специализируем структуру под указатели
template<typename T>
struct FImpl<T*> {
    static int f(T* t) {
        return 1;
    }
};

template<typename T> int f(T t) {
    return FImpl<T>::f(t); // <- теперь мы можем считать эту функцию "частично специализированный"
}

int main(int argc, const char* argv[]) {
    int firstValue = 12;
    std::cout << f(x) << std::endl; // <- здесь используется оригинал FImpl<T>
    
    int* firstPointer = &firstValue;
    std::cout << f(x) << std::endl; // <- а вот тут уже используется специализация FImpl<T*>
}
```

<p>Также, стоит учитывать, что мы не можем назвать шаблонную специализацию наследованием, так как это нарушение одного из 5 принципов SOLID - LSP:</p>

```c++
template<typename T>
struct S {
    void foo();
};

template<> struct S<int> {
    void bar();
};

S<double> sd;
sd.foo();

S<int> si;
si.bar();
```

<p>Специализация может не иметь ничего общего с полной версией.</p>

## 3.5 Шаблоны членов
## 3.6 Параметризация методов и переходники