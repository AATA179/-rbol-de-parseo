Arbol-de-parseo
Crea el árbol de parseo para una cadena ingresada usando la gramatica:
<E> → <E> + <T>
<E> → <T>
<T> → <T> * <U>
<T> → <U>
<U> → num
<U> → ( <E> )
