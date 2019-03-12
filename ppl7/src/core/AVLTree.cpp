/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *******************************************************************************
 * Copyright (c) 2013, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "prolog.h"
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "ppl7.h"


namespace ppl7 {




/*!\class AVLTreeAlgorithm
 * \brief Implementierung des AVL-Algorithmus für binäre Bäume
 *
 * \desc
 * Diese Klasse enthält die Implementierung des AVL-Algorithmus für binäre Bäume.
 * Alle Elemente sind dabei stets sortiert und der Baum ist ausgewogen.
 * Sie dient als Basis für die Template-Klasse AVLTree.
 * \par
 * Die Klasse kann von einer Anwendung nicht direkt verwendet werden. Da der zu verwaltende
 * Datentyp nicht bekannt ist, muss zunächst eine Ableitung erstellt werden und die
 * Vergleichsfunktion AVLTreeAlgorithm::compare reimplementiert werden.
 * \par
 *
 * \attention Die Klasse verwaltet weder Speicher noch hat sie einenen Mutex. Die Anwendung
 * muss sich also selbst um Speicher und Threadsicherheit kümmern.
 *
 */

/*!\var AVLTreeAlgorithm::root
 * \brief Pointer auf die Wurzel des Baums
 */

/*!\var AVLTreeAlgorithm::count
 * \brief Anzahl Knoten im Baum
 */


/*!\class AVLTreeAlgorithm::Iterator
 * \brief %Iterator zum Durchwandern eines AVL-Baums mit der Klasse AVLTreeAlgorithm.
 *
 * \desc
 * Dieser %Iterator wird zum Durchwandern eines AVL-Baums mit der Klasse AVLTreeAlgorithm benötigt.
 *
 * \see
 * - AVLTreeAlgorithm::reset
 * - AVLTreeAlgorithm::getFirst
 * - AVLTreeAlgorithm::getNext
 * - AVLTreeAlgorithm::getLast
 * - AVLTreeAlgorithm::getPrevious
 * - AVLTreeAlgorithm::getCurrent
 *
 */

/*!\class AVLTreeAlgorithm::Node
 * \brief Verwaltungsstruktur für einen Knoten des AVL-Baums
 *
 * \desc
 * Diese Basisklasse enthält Variablen, die zur Verwaltung eines Knotens innerhalb eines AVL-Baums
 * erforderlich sind.
 *
 */

/*!\var AVLTreeAlgorithm::Node::left
 * \brief Zeigt auf den linken Folgeknoten
 */

/*!\var AVLTreeAlgorithm::Node::right
 * \brief Zeigt auf den rechten Folgeknoten
 */

/*!\var AVLTreeAlgorithm::Node::parent
 * \brief Zeigt auf den übergeordneten Knoten
 */

/*!\var AVLTreeAlgorithm::Node::balance
 * \brief Enthält die aktuelle Balance des Knotens
 */



/*!\var AVLTreeAlgorithm::Iterator::current
 * \brief Aktueller Knoten beim Durchwandern des Baums
 */

/*!\var AVLTreeAlgorithm::Iterator::stack
 * \brief Alle Knoten oberhalb von current
 */

/*!\var AVLTreeAlgorithm::Iterator::stack_height
 * \brief Anzahl Knoten im Stack
 */



/*!\brief Konstruktor
 *
 * \desc
 * Im Konstruktor wird der Baum mit NULL initialisiert.
 */
AVLTreeAlgorithm::AVLTreeAlgorithm()
{
	numElements=0;
	root=NULL;
	dupes=false;
}

/*!\brief Destruktor
 *
 * \desc
 * Der Destruktor hat gegenwärtig keine Funktion, da die Klasse selbst keinen Speicher
 * verwaltet.
 */
AVLTreeAlgorithm::~AVLTreeAlgorithm()
{
}


/*!\brief Zwei Elemente des Baums vergleichen
 *
 * \desc
 * Damit Elemente sortiert in den Baum eingehangen werden können, muss eine Möglichkeit
 * bestehen zwei Elemente zu vergleichen. Dies wird mit dieser Methode realisiert.
 * Da jeder Baum andere Daten enthalten kann, muss die Methode für jeden Datentyp
 * reimplementiert werden, was innerhalb des Templates AVLTree auch erfolgt.
 *
 * \param[in] value1 Pointer auf das erste Element.
 *
 * \param[in] value2 Pointer auf das zweite Element.
 *
 * \return Die Funktion muss einen der folgenden 3 Werte zurückliefern:
 * - 0: Ist der Wert in \p value2 identisch mit \p value1, muss 0 zurückgegeben werden.
 * - +1: Ist der Wert in \p value2 größer als der Wert in \p value1, muss +1 zurückgegeben werden
 * - -1: Ist der Wert in \p value2 kleiner als der Wert in \p value1, muss -1 zurückgegeben werden
 * \exception AVLTreeAlgorithm::MissingCompareOperator: Wird geworfen, wenn keine Vergleichsoperatoren
 * implementiert wurden. Bei Verwendung der Template-Klasse AVLTree kann dies nicht vorkommen.
 *
 * \attention Beim Vergleich zweier Strings kann die Funktion \c strcmp nicht direkt
 * verwendet werden, da sie laut Definition Werte kleiner oder größer 0 liefert, aber
 * nicht exakt -1 oder +1.
 *
 * \example
 * Beispiel für eine Implementierung:
 \code
class MyItem : public ppl7::AVLTreeAlgorithm::Node
{
	public:
		MyItem(const char *name) {
			Name=name;
		}
		ppl7::CString Name;
};

class MyTree : public ppl7::AVLTreeAlgorithm
{
	public:
		~MyTree() {
			Clear();
		}
		virtual int	compare(const Node &value1, const Node &value2) const
		{
			const MyItem &v1=static_cast<const MyItem&>(value1);
			const MyItem &v2=static_cast<const MyItem&>(value2);
			if (v1.Name<v2.Name) return -1;
			if (v1.Name>v2.Name) return 1;
			return 0;
		}
}
\endcode
*/
int	AVLTreeAlgorithm::compare(const Node &value1, const Node &value2) const
{
	throw MissingCompareOperator();
}

/*!\brief Duplikate erlauben
 *
 * \desc
 * Mit dieser Funktion kann festgelegt werden, ob Elemente mit gleichem Schlüssel im Baum
 * erlaubt sind. Normalerweise ist dies nicht der Fall. Dabei gilt zu beachten, dass einige
 * Funktionen möglicherweise unerwartet funktionieren. So wird AVLTreeAlgorithm::Find immer nur das
 * erste Element finden können, ebenso AVLTreeAlgorithm::Delete oder AVLTreeAlgorithm::Remove.
 *
 * @param allow Mit \c true werden Duplikate erlaubt, mit \c false nicht. Werden bei einem
 * bereits gefüllten Baum nachträglich Duplikate verboten, hat dies keine Auswirkung auf
 * bereits vorhandene Duplikate.
 */
void AVLTreeAlgorithm::allowDupes(bool allow)
{
	dupes=allow;
}

/*!\brief Anzahl Elemente im Baum
 *
 * \desc
 * Diese Funktion liefert die Anzahl Elemente im Baum zurück.
 *
 * \return Anzahl Elemente, bzw. 0, wenn der Baum leer ist.
 */
size_t AVLTreeAlgorithm::num() const
{
	return numElements;
}

/*!\brief Anzahl Elemente im Baum
 *
 * \desc
 * Diese Funktion liefert die Anzahl Elemente im Baum zurück.
 *
 * \return Anzahl Elemente, bzw. 0, wenn der Baum leer ist.
 */
size_t AVLTreeAlgorithm::count() const
{
	return numElements;
}

/*!\brief Baum als leer kennzeichnen
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der AVL-Baum als leer gekennzeichnet, das heisst
 * der Basisknoten root zeigt auf NULL und der Elementzähler wird auf 0 gesetzt.
 * Die aufrufende Funktion ist dafür verantwortlich, dass der durch den Baum belegte
 * Speicher freigegeben wird.
 */
void AVLTreeAlgorithm::clear()
{
	root=NULL;
	numElements=0;
}

/*!\brief Element hinzufügen
 *
 * \desc
 * Diese Funktion fügt ein neues Element in den Baum ein. Dabei ist sichergestellt,
 * dass der Baum stets sortiert und ausgewogen ist. Im Gegensatz zu CTree erlaub AVLTreeAlgorithm
 * auch mehrere Elemente mit dem gleichen Schlüssel. Dieses Feature muss jedoch explizit
 * durch Aufruf der Funktion AVLTreeAlgorithm::AllowDupes aktiviert werden.
 *
 * \param[in] item Pointer auf den hinzuzufügenden Wert
 * \exception EmptyDataException: Wird geworfen, wenn der Parameter \p item auf NULL zeigt
 * \exception DuplicateItemException: Wird geworfen, wenn es bereits ein Element mit dem
 * gleichen Schlüssel gibt
 */
void AVLTreeAlgorithm::addNode(Node *item)
{
	Node *cur=root;
	Node *node=NULL;
	int cmp=0;
	if (!item) throw EmptyDataException();
	item->right=item->left=NULL;
	item->balance=0;
	if (!root) {
		root=item;
		item->parent=NULL;
		numElements++;
		return;
	}
	while (cur) {
		node=cur;
		cmp=compare(*cur,*item);
		if (cmp==0 && dupes==false) {
			throw DuplicateItemException();
		} else if (cmp<=0) cur=cur->left;
		else cur=cur->right;
	}
	item->parent=node;
	if (cmp>0) {
		node->right=item;
		node->balance++;
	} else {
		node->left=item;
		node->balance--;
	}
	numElements++;
	if (node->balance!=0) upInsert(node);
}

/*!\brief Element aus dem AVL-Baum entfernen
 *
 * \desc
 * Diese Funktion löscht das angegebene Element \p item aus dem Baum. Dabei ist sichergestellt,
 * dass der Baum stets sortiert und ausgewogen ist.
 * \par
 * Mit dieser Funktion wird das Element nur aus dem AVL-Baum entfernt, der durch das Element
 * belegte Speicher wird jedoch nicht freigegeben. Dafür ist die aufrufende Funktion
 * zuständig.
 *
 * \param[in] item Pointer auf den zu entfernenden Knoten
 * \exception NullPointerException: Wird geworfen, wenn \p item auf NULL zeigt
 */
void AVLTreeAlgorithm::eraseNode(Node *item)
{
	if (!item) throw NullPointerException();
	Node *father;
	if((item->left)&&(item->right)) {
		Node *sys;
		if (item->balance<0) {
			// Wenn die Balance -1 ist, tauschen wir den Knoten mit dem am weitesten
			// rechts befindlichen Knoten des linken Teilbaums
			sys=item->left;	// Blatt finden
			while (sys->right) sys=sys->right;
		} else {
			// Wenn die Balance >=0 ist, tauschen wir den Knoten mit dem am weitesten
			// links befindlichen Knoten des rechten Teilbaums
			sys=item->right;	// Blatt finden
			while (sys->left) sys=sys->left;
		}
		swapNodes(item,sys);
		if (item==root) root=sys;
	}
	father=item->parent;
	if((!item->left)&&(!item->right)) {
		if(item==root) {
			//DestroyValue((void*)item->data);
			//Heap->Free(item);
			root=NULL;
			numElements--;
			return;
		} else {
			if(father->left==item) {
				father->left=NULL;
				father->balance++;
			} else {
				father->right=NULL;
				father->balance--;
			}
			upDelete(father);
			//DestroyValue((void*)item->data);
			//Heap->Free(item);
			numElements--;
			return;
		}
	}
	Node *sohn;
	if(item->left)
		sohn=item->left;
	else
		sohn=item->right;
	if(item!=root) {
		sohn->parent=father;
		if(father->left==item) {
			father->left=sohn;
			father->balance++;
		} else {
			father->right=sohn;
			father->balance--;
		}
		upDelete(father);
	} else {
		sohn->parent=NULL;
		root=sohn;
	}
	//DestroyValue((void*)item->data);
	//Heap->Free(item);
	numElements--;
}

/*!\brief Knoten einfügen und Balance herstellen
 *
 * \desc
 * Ein Knoten wird in den AVL-Baum eingefügt und bei Bedarf die Balance wiederhergestellt.
 *
 * @param node Neuer Knoten
 */
void AVLTreeAlgorithm::upInsert(Node *node)
{
	if (((node->balance==-1) || (node->balance==1)) && (node!=root)) {
		if (node->parent->left==node)
			node->parent->balance--;
		else
			node->parent->balance++;
		upInsert(node->parent);
		return;
	}
	if (node->balance==-2) {
		if (node->left->balance==-1) {
			rotate(node);
			return;
		} else {
			rotate(node->left);
			rotate(node);
			return;
		}
	}
	if (node->balance==2) {
		if (node->right->balance==1) {
			rotate(node);
			return;
		} else {
			rotate(node->right);
			rotate(node);
			return;
		}
	}
}

/*!\brief Balance nach Löschen eines Knotens wiederherstellen
 *
 * \desc
 * Mit dieser Funktion wird die Balance des Baums nach dem Löschen eines Knotens
 * wiederhergestellt.
 *
 * @param node Zu löschender Knoten
 */
void AVLTreeAlgorithm::upDelete(Node *node)
{
	Node *parent=node->parent;
	if ((node->balance==-1) || (node->balance==1)) return;
	if ((node==root) && (node->balance==0)) return;
	if (node==root) {
		if(node->balance==-2) {
			if (node->left->balance<=0) rotate(node);
			else {
				node=rotate(node->left);
				rotate(node);
			}
		} else {
			if (node->right->balance>=0) rotate(node);
			else {
				node=rotate(node->right);
				rotate(node);
			}
		}
		return;
	}
	if (node->balance==2) {
		switch(node->right->balance) {
			case 0:
				rotate(node);
				return;
			case 1:
				upDelete(rotate(node));
				return;
			case -1:
			rotate(node->right);
			upDelete(rotate(node));
			return;
		}
	}
	if (node->balance==-2) {
		switch(node->left->balance) {
			case 0:
				rotate(node);
				return;
			case -1:
			upDelete(rotate(node));
			return;
			case 1:
				rotate(node->left);
				upDelete(rotate(node));
				return;
		}
	}
	if (parent->left==node) {
		parent->balance++;
		if (parent->balance<2) {
			upDelete(parent);
			return;
		}
		switch(parent->right->balance) {
			case 0:
				rotate(parent);
				return;
			case 1:
				upDelete(rotate(parent));
				return;
			case -1:
				rotate(parent->right);
				upDelete(rotate(parent));
				return;
		}
	}
	if (parent->right==node) {
		parent->balance--;
		if (parent->balance>-2) {
			upDelete(parent);
			return;
		}
		switch (parent->left->balance) {
			case 0:
				rotate(parent);
				return;
			case -1:
				upDelete(rotate(parent));
				return;
			case 1:
				rotate(parent->left);
				upDelete(rotate(parent));
				return;
		}
	}
}

/*!\brief Rotation druchführen
 *
 * \desc
 * Ein Knoten wird rotiert, um die Balance wieder herzustellen
 *
 * @param kn Knoten, der routiert werden soll
 * @return Pointer auf Kindknoten
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::rotate(Node *kn)
{
	Node *child;
	if (kn->balance<0) {
		child=kn->left;
		kn->left=child->right;
		if (child->right) child->right->parent=kn;
		child->right=kn;
		child->parent=kn->parent;
		kn->parent=child;
		if (child->parent) {
			if (child->parent->left==kn)
				child->parent->left=child;
			else
				child->parent->right=child;
		} else
			root=child;
		if (kn->balance==-1) {
			if(child->balance==1) {
				child->balance=2;
				kn->balance=0;
				return child;
			}
			if (child->balance==-1)
				kn->balance=1;
			else
				kn->balance=0;
			child->balance=1;
			return child;
		}
		if (kn->balance==-2) {
			if (child->balance==-1) {
				kn->balance=child->balance=0;
				return child;
			}
			if(child->balance==0) {
				kn->balance=-1;
				child->balance=1;
				return child;
			}
			if (child->balance==-2) {
				kn->balance=1;
				child->balance=0;
				return child;
			}
		}
	} else {
		child=kn->right;
		kn->right=child->left;
		if(child->left) child->left->parent=kn;
		child->left=kn;								// Step 3
		child->parent=kn->parent;					// Step 4
		kn->parent=child;							// Step 5
		if(child->parent) {
			if (child->parent->left==kn)
				child->parent->left=child;
			else
				child->parent->right=child;
		} else
			root=child;
		if (kn->balance==1) {
			if (child->balance==-1) {
				child->balance=-2;
				kn->balance=0;
				return child;
			}
			if (child->balance==1)
				kn->balance=-1;
			else
				kn->balance=0;
			child->balance=-1;
			return child;
		}
		if (kn->balance==2) {
			if (child->balance==1) {
				kn->balance=child->balance=0;
				return child;
			}
			if (child->balance==0) {
				kn->balance=1;
				child->balance=-1;
				return child;
			}
			if (child->balance==2) {
				kn->balance=-1;
				child->balance=0;
				return child;
			}
		}
	}
	return child;
}

/*!\brief Zwei Knoten miteinander tauschen
 *
 * \desc
 * Zwei AVL-Knoten werden miteinander vertauscht
 *
 * @param item1 Pointer auf ersten Knoten
 * @param item2 Pointer auf zweiten Knoten
 */
void AVLTreeAlgorithm::swapNodes(Node *item1, Node *item2)
{
	signed char	balance;
	Node *left, *right, *parent;
	// Kopieren der Parameter von item1 nach tmp
	left=item1->left;
	right=item1->right;
	balance=item1->balance;
	parent=item1->parent;
	// Kopieren der Parameter von item2 nach item1
	item1->left=item2->left;
	item1->right=item2->right;
	item1->balance=item2->balance;
	item1->parent=item2->parent;
	// Kopieren der Parameter von tmp nach item2
	item2->left=left;
	item2->right=right;
	item2->balance=balance;
	item2->parent=parent;
	// Falls die Nodes direkt nebeneinander liegen, könnten die Child-Pointer auf sich selbst zeigen.
	// Das müssen wir korrigieren
	if (item1->left==item1) item1->left=item2;
	if (item1->right==item1) item1->right=item2;
	if (item2->left==item2) item2->left=item1;
	if (item2->right==item2) item2->right=item1;
	// Pointer der Kinder korrigieren
	if (item1->left) item1->left->parent=item1;
	if (item1->right) item1->right->parent=item1;
	if (item2->left) item2->left->parent=item2;
	if (item2->right) item2->right->parent=item2;
	// Pointer der Parents korrigieren
	if (item1->parent) {
		if (item1->parent->left==item2) item1->parent->left=item1;
		else if (item1->parent->right==item2) item1->parent->right=item1;
	}
	if (item2->parent) {
		if (item2->parent->left==item1) item2->parent->left=item2;
		else if (item2->parent->right==item1) item2->parent->right=item2;
	}
}

/*!\brief Wert im Baum finden
 *
 * \desc
 * Mit dieser Funktion wird der Wert \p value innerhalb des Baums gesucht und dessen
 * Baumelement (TREEITEM) zurückgegeben.
 *
 * \param[in] value Der zu suchende Wert
 * \return Wird der Wert im Baum gefunden, gibt die Funktion einen Pointer auf den
 * Wert des Baum-Elements zurück. Falls nicht, wird NULL zurückgegeben.
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::findNode(const Node *value) const
{
	if (!value) throw IllegalArgumentException();
	Node *cur=root;
	while (cur) {
		int cmp=compare(*cur,*value);
		if (cmp==-1) cur=cur->left;
		else if (cmp==1) cur=cur->right;
		else return cur;
	}
	return NULL;
}

/*!\brief Konstruktor des Iterators
 *
 * \desc
 * Initialisiert den %Iterator
 */
AVLTreeAlgorithm::Iterator::Iterator()
{
	for (int i=0;i<AVL_MAX_HEIGHT;i++) {
		stack[i]=0;
	}
	current=NULL;
	stack_height=0;
}

/*!\brief %Iterator auf erstes Element zurücksetzen
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der angegebene %Iterator \p it zurückgesetzt.
 * Wird als nächstes AVLTreeAlgorithm::getNext aufgerufen, wird das erste Element des
 * Baumes zurückgegeben, bei Aufruf von AVLTreeAlgorithm::getPrevious das letzte.
 *
 * @param it Iterator
 */
void AVLTreeAlgorithm::reset(Iterator &it) const
{
	it.current=NULL;
	it.stack_height=0;
}

/*!\brief Erstes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird ein Pointer auf das erste Element des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Pointer auf das erste Element des Baums oder NULL, wenn der Baum leer ist
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getFirst(Iterator &it) const
{
	it.current=NULL;
	it.stack_height=0;
	Node *node;
	node = root;
	if (node != NULL)
		while (node->left != NULL) {
    		it.stack[it.stack_height++] = node;
			node = node->left;
		}
	it.current = node;
	return node;
}

/*!\brief Nächstes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird ein Pointer auf das nächste Element des Baums zurückgeliefert.
 * Somit kann der Baum sortiert vorwärts durchwandert werden.
 *
 * @param it %Iterator
 *
 * \return Pointer auf das nächste Element des Baums oder NULL, wenn keine weiteren
 * Elemente vorhanden sind.
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getNext(Iterator &it) const
{
	Node *node=it.current;
	if (node == NULL) {
		return getFirst(it);
	} else if (node->right != NULL) {
		it.stack[it.stack_height++] = node;
		node = node->right;
		while (node->left != NULL) {
			it.stack[it.stack_height++] = node;
			node = node->left;
		}
	} else {
		Node *tmp;
		do {
			if (it.stack_height == 0) {
				it.current = NULL;
				return NULL;
			}
			tmp=node;
			node = it.stack[--it.stack_height];
		} while (tmp == node->right);
	}
	it.current=node;
	return node;
}

/*!\brief Letztes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird ein Pointer auf das letzte Element des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Pointer auf das letzte Element des Baums oder NULL, wenn der Baum leer ist
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getLast(Iterator &it) const
{
	Node *node;
	it.stack_height = 0;
	node = root;
	if (node != NULL)
		while (node->right != NULL) {
			it.stack[it.stack_height++] = node;
			node = node->right;
		}
	it.current = node;
	return node;
}

/*!\brief Vorheriges Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird ein Pointer auf das vorherige Element des Baums zurückgeliefert.
 * Somit kann der Baum sortiert rückwärts durchwandert werden.
 *
 * @param it %Iterator
 *
 * \return Pointer auf das vorherige Element des Baums oder NULL, wenn keine weiteren
 * Elemente vorhanden sind.
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getPrevious(Iterator &it) const
{
	Node *node;
	node = it.current;
	if (node == NULL) {
		return getLast(it);
	} else if (node->left != NULL) {
		it.stack[it.stack_height++] = node;
		node = node->left;
		while (node->right != NULL) {
			it.stack[it.stack_height++] = node;
			node=node->right;
		}
	} else {
		Node *y;
		do {
			if (it.stack_height == 0) {
				it.current = NULL;
				return NULL;
			}
			y = node;
			node = it.stack[--it.stack_height];
		} while (y == node->left);
	}
	it.current = node;
	return node;
}

/*!\brief Aktuelles Element des Baums
 *
 * \desc
 * Mit dieser Funktion wird ein Pointer auf das aktuelle Element des Baums zurückgeliefert.
 * Dabei wird der Pointer nicht verändert.
 *
 * @param it %Iterator
 *
 * \return Pointer auf das aktuelle Element des Baums oder NULL, wenn kein
 * Element mehr vorhanden ist.
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getCurrent(Iterator &it) const
{
	return it.current;
}

/*!\brief Basisknoten zurückgeben
 *
 * \desc
 * Diese Funktion liefert einen Pointer auf den Basisknoten (root) des Baumes zurück.
 *
 * @return Pointer auf Basisknoten oder NULL, wenn der Baum leer ist
 */
AVLTreeAlgorithm::Node *AVLTreeAlgorithm::getRoot() const
{
	return root;
}



/*!\class AVLTree
 * \brief Template für AVL-Bäume
 *
 * \desc
 * Hierbei handelt es sich um ein generisches Template für binäre Bäume, die nach dem AVL-Algorithmus
 * aufgebaut sind.
 * \par
 * Die Datentypen für Schlüssel und Wert sind getrennt voneinander beliebig definierbar. Einzige
 * Voraussetzung für den Schlüssel ist, dass er über die Vergleichoperatoren "<", ">" und "="
 * verfügt.
 *
 * \example
 * Das nachfolgende Beispiel zeigt, wie das Template verwendet werden kann, um einen Baum von
 * Key-Value-Paaren zu verwalten, wobei sowohl Schlüssel als auch Wert als String definiert sind.
 * \code
#include <ppl7.h>
int main(int argc, const char**argv)
{
	ppl7::AVLTree<ppl7::String, ppl7::String> myMap;
	myMap.add(L"key1",L"value1");
	myMap.add(L"other",L"value2");
	myMap.add(L"findme",L"success");
	myMap.add(L"key3",L"value3");
	myMap.add(L"abc",L"value4");
	try {
		const ppl7::String &res=myMap.find(L"findme");
		res.printnl();
	} catch (ppl7::ItemNotFoundException) {
		printf ("Element wurde nicht gefunden\n");
	}

	// Iterator definieren
    ppl7::AVLTree<ppl7::String, ppl7::String>::Iterator it;

    // Alle Elemente per Iterator ausgeben
    myMap.reset(it);
    while (myMap.getNext(it)) {
        printf ("Found Key >>%s<< with Value >>%s<<\n",
            (const char*)it.key(), (const char*)it.value());
    }
	return 0;
}
 * \endcode
 *
 *
 */

/*!\class AVLTree::TreeItem
 * \brief Einzelnes Baumelement
 *
 * \desc
 * Mit dieser Klasse wird ein einzelnes Element des AVL-Baums repräsentiert.
 */

/*!\var AVLTree::TreeItem::key
 * \brief Schlüsselwert des Elements
 */

/*!\var AVLTree::TreeItem::value
 * \brief Datenwert des Elements
 */

/*!\fn void *AVLTree::TreeItem::operator new (size_t, void *p)
 * \brief Speicher für den Knoten zuweisen
 *
 * \desc
 * Der Speicher für die Knoten wird über einen Heap der Klasse AVLTree verwaltet und
 * über diesen Operator der Klasse zugewiesen.
 * Die AVLTree Klasse sorgt dafür, dass beim Löschen eines Knotens der Destruktor der
 * Klasse aufgerufen wird, damit auch der Speicher des Schlüssels und des Wertes
 * freigegeben wird.
 */



/*!\var AVLTree::MyHeap
 * \brief Speicherverwaltung der Knoten
 *
 * \desc
 * Dieser Heap wird zur Speicherverwaltung der Knoten verwendet und verhindert eine zu starke
 * Fragmentierung des Speichers.
 *
 */

/*!\fn AVLTree::AVLTree()
 * \brief Konstruktor der Klasse
 *
 * \desc
 * Der Konstruktor initialisiert die Speicherverwaltung innerhalb des Heaps.
 */

/*!\fn AVLTree::~AVLTree()
 * \brief Destruktor der Klasse
 *
 * \desc
 * Der Destruktor ruft die Funktion AVLTree::clear auf, wodurch der komplette durch
 * den Baum und seine Elemente belegte Speicher wieder freigegeben wird.
 */

/*!\fn size_t AVLTree::capacity() const
 * \brief Aktuelle Kapazität des AVL-Baums
 *
 * \desc
 * Gibt zurück, wieviele Elemente der AVL-Baum verwalten kann, ohne dass neuer Speicher
 * allokiert werden muss.
 *
 * \return Anzahl Elemente
 */

/*!\fn void AVLTree::reserve(size_t num)
 * \brief Speicher reservieren
 *
 * \desc
 * Mit dieser Funktion kann vorab Speicher für eine bestimmte Anzahl Elemente reserviert werden.
 * Der Aufruf dieser Funktion ist immer dann sinnvoll, wenn schon vorher bekannt ist, wieviele
 * Elemente benötigt werden, insbesondere, wenn sehr viele Elemente benötigt werden.
 *
 * @param num Anzahl Elemente, für die Speicher vorab allokiert werden soll
 *
 * \note Falls schon Speicher allokiert wurde, wird die Anzahl der bereits allokierten Elemente
 * mit \p num verrechnet und nur die Differenz zusätzlich reserviert.
 */

/*!\fn size_t AVLTree::itemSize() const
 * \brief Größe eines Verwaltungsknotens
 *
 * \desc
 * Liefert die Anzahl Bytes zurück, die für die Verwaltung eines einzelnen Knotens erforderlich sind.
 *
 * \return Anzahl Byte
 */


/*!\fn void AVLTree::clearRecursive(TreeItem *item)
 * \brief Rekursiv die Destruktoren aller Elemente aufrufen
 *
 * \desc
 * Diese interne Funktion wird von AVLTree::clear aufgerufen, um rekursiv die Destruktoren aller
 * Elemente aufzurufen. Somit ist sichergestellt, dass beim Löschen des Baums nicht nur der
 * Verwaltungsspeicher für die Knoten innerhalb des Heaps freigegeben wird, sondern auch
 * der Speicher der durch die Schlüssel und Werte der Knoten belegt wird.
 */

/*!\fn void AVLTree::add(const K &key, const T &value)
 * \brief Element hinzufügen
 *
 * \desc
 * Mit dieser Funktion wird ein neues Element dem Baum hinzugefügt. Sowohl der Schlüssel \p key
 * als auch der Wert \p value werden dabei kopiert.
 *
 * \param key Referenz auf den Schlüssel des Elements
 * \param value Referenz auf den Wert des Elements
 *
 * \exception DuplicateItemException: Wird geworfen, wenn es bereits ein Element mit dem
 * gleichen Schlüssel gibt
 *
 */

/*!\fn T& AVLTree::find(const K &key) const
 * \brief Element finden
 *
 * \desc
 * Mit dieser Funktion wird das Element mit dem Schlüssel \p key im Baum gesucht und dessen
 * Wert zurückgegeben.
 *
 * \param key Referenz auf den gesuchten Schlüssel
 * \return Gibt eine Referenz auf den Wert des Elements zurück. Der Wert kann durch die
 * aufrufende Funktion verändert werden.
 * \exception ItemNotFoundException: Wird geworfen, wenn der gesuchte Schlüssel im Baum nicht vorhanden ist.
 *
 */

/*!\fn bool AVLTree::exists(const K &key) const
 * \brief Prüft, ob ein bestimmter Schlüssel vorhanden ist.
 *
 * \desc
 * Mit dieser Funktion wird geprüft, ob ein Element mit dem Schlüssel \p key im Baum vorhanden ist.
 *
 * \param key Referenz auf den gesuchten Schlüssel
 * \return Gibt \c true zurück, wenn der Schlüssel vorhanden ist, sonst \c false.
 */


/*!\fn T& AVLTree::operator[](const K &key) const
 * \brief Element finden
 *
 * \desc
 * Mit diesem Operator wird das Element mit dem Schlüssel \p key im Baum gesucht und dessen
 * Wert zurückgegeben.
 *
 * \param key Referenz auf den gesuchten Schlüssel
 * \return Gibt eine Referenz auf den Wert des Elements zurück. Der Wert kann durch die
 * aufrufende Funktion verändert werden.
 * \exception ItemNotFoundException: Wird geworfen, wenn der gesuchte Schlüssel im Baum nicht vorhanden ist.
 */

/*!\fn void AVLTree::erase(const K &key)
 * \brief Element löschen
 *
 * \desc
 * Mit dieser Funktion wird das Element mit dem Schlüssel \p key aus dem Baum gelöscht und der dadurch
 * referenzierte Speicher gelöscht.
 *
 * \param key Zu löschender Schlüssel
 * \exception ItemNotFoundException: Wird geworfen, wenn der gesuchte Schlüssel im Baum nicht vorhanden ist.
 */

/*!\fn bool AVLTree::getFirst(Iterator &it) const
 * \brief Erstes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird das erste Key-Value-Paar des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Gibt \c true zurück, wenn ein Element gefunden wurde, sonst  \c false.
 */

/*!\fn bool AVLTree::getNext(Iterator &it) const
 * \brief Nächstes Element auslesen
 *
 * \desc
 * Mit dieser Funktion wird das nächste Key-Value-Paar des Baums zurückgeliefert.
 *
 * @param it %Iterator
 * \return Gibt \c true zurück, wenn ein Element gefunden wurde, sonst  \c false.
 * \example
 * \code
	ppl7::AVLTree<ppl7::String, ppl7::String> myMap;
	myMap.add(L"key1",L"value1");
	myMap.add(L"other",L"value2");
	myMap.add(L"findme",L"success");
	myMap.add(L"key3",L"value3");
	myMap.add(L"abc",L"value4");
	// Iterator definieren
	ppl7::AVLTree<ppl7::String, ppl7::String>::Iterator it;
	myMap.reset(it);
	while (myMap.getNext(it)) {
		printf ("Found Key >>%s<< with Value >>%s<<\n",
			(const char*)it.key(), (const char*)it.value());
	}
 * \endcode
 */

/*!\fn bool AVLTree::getLast(Iterator &it) const
 * \brief Letztes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird das letzte Key-Value-Paar des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Gibt \c true zurück, wenn ein Element gefunden wurde, sonst  \c false.
 */

/*!\fn bool AVLTree::getPrevious(Iterator &it) const
 * \brief Verhergehendes Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird das vorhergehende Key-Value-Paar des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Gibt \c true zurück, wenn ein Element gefunden wurde, sonst  \c false.
 */

/*!\fn bool AVLTree::getCurrent(Iterator &it) const
 * \brief Aktuelles Element aus dem Baum
 *
 * \desc
 * Mit dieser Funktion wird das aktuelle Key-Value-Paar des Baums zurückgeliefert.
 *
 * @param it %Iterator
 *
 * \return Gibt \c true zurück, wenn ein Element gefunden wurde, sonst  \c false.
 */



/*!\class AVLTree::Iterator
 * \brief %Iterator für AVLTree
 *
 * \desc
 * Dieser %Iterator wird zum Durchwandern eines AVLTree benötigt.
 *
 * \example
 * Beispiel zur Verwendung des Iterators:
 * \code
	ppl7::AVLTree<ppl7::String, ppl7::String> myMap;
	myMap.add(L"key1",L"value1");
	myMap.add(L"other",L"value2");
	myMap.add(L"findme",L"success");
	myMap.add(L"key3",L"value3");
	myMap.add(L"abc",L"value4");
	// Iterator definieren
	ppl7::AVLTree<ppl7::String, ppl7::String>::Iterator it;
	myMap.reset(it);
	while (myMap.getNext(it)) {
		printf ("Found Key >>%s<< with Value >>%s<<\n",
			(const char*)it.key(), (const char*)it.value());
	}
 * \endcode
 */

/*!\fn const K& AVLTree::Iterator::key() const
 * \brief Schlüsselwert auslesen
 *
 * \desc
 * Mit dieser Funktion wird der Wert des aktuellen Schlüssels ausgelesen.
 *
 * \return Gibt eine Referenz auf den Schlüssel zurück
 *
 * \exception NullPointerException: Wird geworfen, wenn der Iterator auf kein gültiges Element
 * eines AVLTree zeigt.
 */

/*!\fn T& AVLTree::Iterator::value() const
 * \brief Datenwert des Elements auslesen
 *
 * \desc
 * Mit dieser Funktion wird der Datenwert des aktuellen Schlüssels ausgelesen.
 *
 * \return Gibt eine Referenz auf den Datenwert zurück
 *
 * \exception NullPointerException: Wird geworfen, wenn der Iterator auf kein gültiges Element
 * eines AVLTree zeigt.
 */


}	// EOF namespace ppl7
