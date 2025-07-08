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
<p>Данная функция работает не для всех типов. Но как сделать так, чтобы было иначе? Все просто: добавить template:</p>

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
<p>Что же тут неверно?:</p>
<ul>
<li>Корнер-кейс x < 2 не сработает для типа float</li>
<li>В случае, если мы работаем с матрицами, то T result = 1 просто так не сработает</li>
</ul>
<p>Тогда встает резонный вопрос: как это исправить? Ответ очевидный: мы должно подогнать алгоритм под типы, а не наоборот! Рассмотрим два варианта изменения этой функции:</p>
<ul>
<li>Добавим специальный default_id_trait</li>
<li>Вынесем основную часть функции - возведение в степень - и определим требования для типа T</li>
</ul>

<p>Рассмотрим второй вариант:</p>

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

<p>Вариант рабочий. Однако, есть один нюанс. А как точно понять, что T - копируемый тип (таким обазом, что мы его можем перемножать)? Для проверки этого добавим concept к нашей функции через requires:</p>

```c++
template <typename T> concept multiplicative = requires(T t) {
    { t *= t } -> std::convertible_to<T>;
};

template<typename T>
T do_nth_power(T x, T result, unsigned n) 
requires multiplicative<T> && std::copyable<T>
```

<p>Достаточно спорный момент. Можно много глаголить, но лучше всего специализировать typename стандартными методами STL (например, std::intergral)</p>

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


