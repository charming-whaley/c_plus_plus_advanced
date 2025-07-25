# Лекция 2. Шаблоны функций

## 2.1 Обобщенное программирование

<p>Рассмотрим следующую функцию:</p>

```c++
unsigned nth_power(unsigned x, unsigned n) {
    unsigned result = 1;
    if (x < 2 || n == 1)
        return x;
    while (n > 0) {
        if ((n & 0x1) == 0x1) { // проверка n на четность 
            result *= x;
            n -= 1;
        } else {
            x *= x;
            n /= 2;
        }
    }
    return result;
}
```

<p>Текущая функция работает не для всех типов, а только для тех, что являются <b>unsigned</b>, те такие типы как, например, <b>float</b> здесь неуместны. Для того, чтобы исправить эту проблему, мы можем добавить стандартный <b>template</b> к нашей функции:</p>

```c++
template<typename T> 
T nth_power(T x, unsigned n) {
    T result = 1;
    if (x < 2 || n == 1)
        return x;
    while (n > 0) {
        if ((n & 0x1) == 0x1) { // проверка n на четность 
            result *= x;
            n -= 1;
        } else {
            x *= x;
            n /= 2;
        }
    }
    return result;
}
```

<p>Стало намного лучше. Однако, в такой реализации функции у нас встречаются следующие банальные проблемы: </p>

<ul>
<li>Корнер-кейс для <b>x</b> не сработает для типа <b>float</b></li>
<li>Если наш <b>T</b> будет матрицей? В таком случае, <b>T result = 1;</b> будет некорректным</li>
</ul>

<p>В таком случае возникает банальный вопрос: как исправить нашу функцию так, чтобы эти две проблемы исчезли? На самом деле очень просто: подогнать наш алгоритм под типы. Для этого мы можем воспользоваться одним из двух следующих методов:</p>
<ul>
<li>Добавить специальный <b>default_id_trait</b> в <b>template</b></li>
<li>Вынести главный алгоритм нашей функции в другю функцию и подогнать наш <b>T</b> под специальные "требования"</li>
</ul>

<p>Первый вариант не совсем очевидный, плюс, не подходит под современные стандарты STL. Поэтому, рассмотрим второ вариант:</p>

```c++
template<typename T>
T do_nth_power(T x, T result, unsigned n) {
    while (n > 0) {
        if ((n & 0x1) == 0x1) { // проверка n на четность 
            result *= x;
            n -= 1;
        } else {
            x *= x;
            n /= 2;
        }
    }
    return result;
}

unsigned nth_power(unsigned x, unsigned n) {
    if (x < 2u || n == 1u)
        return x;
    return do_nth_power<unsigned>(x, 1u, n);
}
```

<p>Также стоит не забыть про то, что мы не указали явно, что <b>T</b> - это копируемый тип (таким образом, чтобы его можно было перемножать). Для этого введем специальный <b>concept</b>:</p>

```c++
template <typename T> concept multiplicative = requires(T t) {
    { t *= t } -> std::convertible_to<T>;
};

template<typename T>
T do_nth_power(T x, T result, unsigned n) 
requires multiplicative<T> && std::copyable<T>
```

## 2.2 Инстанцирование

<p>Инстанцированием мы называем процесс порождения экземпляра специализации</p>

```c++
template<typename T>
T max(T x, T y) {
    return x > y ? x : y;
}

...

max<int>(2, 3)
```

## 2.3 Специализация

<p>Помимо инстанцирования, шаблонная функция или структура может быть явно специализирована:</p>

```c++
template <typename T> T max(T x, T y) { ... }
template <> int max<int>(int x, int y) { ... }
template <> float max<float>(float x, float y) { ... }
```

<p>Важные правила специализации:</p>
<ul>
<li>Явное инстанцирование единожды в программе</li>
<li>Явная специализация единожды в программе</li>
<li>Явное инстанцирование должно следовать за явной специализацией</li>
</ul>

```c++
template<typename T> T max(T x, T y) { ... }
template <> int max<int>(int x, int y) { ... }
template int max<int>(int, int);
```

<p>Также стоит учитывать, что явная специализация может войти в конфликт с инстанцированием:</p>

```c++
template<typename T> T max(T x, T y);

// Указываем явную специализацию
template <> double max(double x, double y) {
    return 42.0;
}

// Никакой implicit instantiation
int foo() {
    return max<double>(2.0, 3.0);
}

// implicit instantionation нужен
int bar() {
    return max<int>(2, 3);
}

// ошибка: мы уже породили эту специализацию
template <> int max(int x, int y) {
    return 42;
}
```

<p>Отсюда вытекает частный случай явной специализации - её запрет:</p>

```c++
template <typename T> void foo(T*);

template<> void foo<char>(char*) = delete;
template<> void foo<void>(void*) = delete;
```

## 2.4 Non-type параметры

<p>Non-type параметры - это структурные типы (т.е. скалярные типы, lvalue-ссылки и структуры, у которых все поля и базовые классы <b>public</b> и не <b>mutable</b></p>

```c++
struct Pair { int x, y; }

struct MoreComplex : public Pair {
    int arr[3]{ 1, 2, 3 };
    int z = 1;
}

template<int N, int* PN, int& RN, MoreComplex M> int buz() {
    return N + *PN + RN + M.z + M.arr[0];
}
```

## 2.5 Вывод типов

<p>Для параметров, являющихся типами, работает вывод типов</p>

```c++
int x = max(1, 2); // -> int max<int>(int, int);
```

<p>Шаблонный тип аргумента может быть уточнен ссылкой или указателем и cv-квалификатором:</p>

```c++
template<typename T> T max(const T& x, const T& y);
int a = max(2, 3); // -> int max<int>(const int&, const int&);
```

<p>В некоторых случаях у нас нет контекста вывода, а поэтому мы можем указать наобходимое и положиться на вывод остального:</p>

```c++
template<typename DstT, typename SrcT>
DstT implicit_cast(SrcT const & x) {
    return x;
}

double value = implicit_cast(-1); // <- ошибка
double value = implicit_cast<double, int>(-1); // <- ok
double value = implicit_cast<double>(-1); // <- ok
```

<p>Допустим у нас есть параметр по умолчанию. Тогда, нам нужно будет указать его тип:</p>

```c++
template<typename T> void foo(T x = 1.0);

foo(1); // <- ok
foo<int>(); // <- ok
foo(); // <- ошибка
```

<p>Однако, мы можем это легко исправить:</p>

```c++
template<typename T = double> void foo(T x = 1.0);

foo(1); // <- ok
foo<int>(); // <- ok
foo(); // <- ok
```

## 2.6 Перегрузка

<p>На <u>Лекции 1. Строки</u> была поставлена задача написать перегрузку оператора <b>==</b>. Вот один из вариантов такой перегрузки:</p>

```c++
template<typename CharT, typename Traits, typename Alloc>
bool operator==(const basic_string<CharT, Traits, Alloc>& lhs, 
                const basic_string<CharT, Traits, Alloc>& rhs) {
    return lhs.compare(rhs) == 0;
}
```

<p>Но у этого есть один очень серьезный недостаток: допустим, мы хотим справнить ("Hello" == str). В такое случае мы создаем лишнюю копию. Решением этой проблемы будет банальная перегрузка оператора <b>==</b>:</p>

```c++
// Ниже приведен вариант такой перегрузки в стандартной библиотеке C++

template<typename CharT, typename Traits, typename Alloc>
bool operator==(const basic_string<CharT, Traits, Alloc>& lhs, 
                const basic_string<CharT, Traits, Alloc>& rhs) {
    return lhs.compare(rhs) == 0;
}

template<typename CharT, typename Traits, typename Alloc>
bool operator==(const CharT* lhs, const basic_string<CharT, Traits, Alloc>& rhs) {
    return rhs.compare(lhs) == 0;
}

template<typename CharT, typename Traits, typename Alloc>
bool operator==(const basic_string<CharT, Traits, Alloc>& lhs, const CharT* rhs) {
    return lhs.compare(rhs) == 0;
}
```

<p>Вот общие правила перегрузки в C++:</p>
<ol>
<li>Выбирается множество перегруженных имён</li>
<li>Выбирается множество кандидатов</li>
<li>Из множества кандидатов выбираются <b>жинеспособные кандидаты</b></li>
<li>Лучший из <b>жинеспособных кандидатов</b> выбирается на основании цепочек неявных преобразований для каждого параметра</li>
<li>Если лучший кандидат существует и является единственным, то перегрузка разрешена успешно, иначе ill-formed</li>
</ol>

<p>Также стоит учитывать, что поиск Кёнига в пространствах имён не сработает для шаблонных функций:</p>

```c++
namespace N {
    struct A;
    template<typename T> int f(A*);
};

int g(N::A *a) {
    int i = f<int>(a); // <- ошибка, < здесь оператор меньше 
    return i;
}
```

<p>Вот как можно это исправить:</p>

```c++
namespace N {
    struct A;
    template<typename T> int f(A*);
};

template<typename T> void f(int); // <- можно добавить любой параметр

int g(N::A *a) {
    int i = f<int>(a); // <- ok 
    return i;
}
```
