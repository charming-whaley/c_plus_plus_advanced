# Лекция 2. Шаблоны функций

## Обобщенное программирование

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

<p>Текущая функция работает не для всех типов, а только для тех, что являются <b>unsigned</b>, те такие типы как, например, float здесь неуместны. Для того, чтобы исправить эту проблему, мы можем добавить стандартный <b>template</b> к нашей функции:</p>

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
<li>Корнер-кейс для x не сработает для типа <b>float</b></li>
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

## Инстанцирование

<p>Инстанцированием мы называем процесс порождения экземпляра специализации</p>

```c++
template<typename T>
T max(T x, T y) {
    return x > y ? x : y;
}

...

max<int>(2, 3)
```

## Специализация

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
