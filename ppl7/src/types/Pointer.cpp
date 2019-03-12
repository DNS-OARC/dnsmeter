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


#include "ppl7.h"

namespace ppl7 {


/*!\class Pointer
 * \ingroup PPLGroupDataTypes
 * \brief %Pointer-Klasse
 *
 * \desc
 * Diese Klasse kann zur Darstellung eines beliebigen %Pointers verwendet werden.
 * Mittels Konstruktor, Operator "=" oder der Funktion Pointer::set kann der %Pointer
 * gesetzt werden, mit Pointer::ptr und verschiedenen Operatoren kann er wieder
 * ausgelesen werden.
 *
 */

/*!\brief Konstruktor der Klasse
 *
 * \desc
 * %Pointer wird mit NULL initialisiert
 */
Pointer::Pointer()
{
	myptr=NULL;
}

/*!\brief Copy-Konstruktor
 *
 * \desc
 * %Pointer übernimmt den Wert von \p other.
 *
 * @param other Andere %Pointer-Instanz
 * @return
 */
Pointer::Pointer(const Pointer &other)
{
	myptr=other.myptr;
}

/*!\brief Konstruktor mit Angabe eines %Pointers
 *
 * \desc
 * Mit diesem Konstruktor wird der Wert von \p ptr übernommen.
 *
 * @param ptr beliebiger Pointer
 * @return
 */
Pointer::Pointer(void *ptr)
{
	myptr=ptr;
}

/*!\brief Ist %Pointer Null?
 *
 * \desc
 * Diese Funktion liefert \c true zurück, wenn der Pointer gegenwärtig auf NULL zeigt.
 * @return Gibt \c true oder \c false zurück.
 */
bool Pointer::isNull() const
{
	if (myptr==NULL) return true;
	return false;
}

/*!\brief %Pointer als "const void *" auslesen
 *
 * \desc
 * Mit dieser Fuktion wird der %Pointer ausgelesen
 *
 * \return %Pointer als "const void *"
 */
const void *Pointer::ptr() const
{
	return myptr;
}

/*!\brief %Pointer als "const void *" auslesen
 *
 * \desc
 * Mit diesem Operator wird der %Pointer ausgelesen
 *
 * \return %Pointer als "const void *"
 */
Pointer::operator const void*() const
{
	return (const void*)myptr;
}

/*!\brief %Pointer als "const char *" auslesen
 *
 * \desc
 * Mit diesem Operator wird der %Pointer ausgelesen
 *
 * \return %Pointer als "const char *"
 */
Pointer::operator const char*() const
{
	return (const char*)myptr;
}

/*!\brief %Pointer als "const unsigned char *" auslesen
 *
 * \desc
 * Mit diesem Operator wird der %Pointer ausgelesen
 *
 * \return %Pointer als "const unsigned char *"
 */
Pointer::operator const unsigned char*() const
{
	return (const unsigned char*)myptr;
}

/*!\brief %Pointer setzen
 *
 * \desc
 * Mit dieser Funktion wird der Wert des %Pointers gesetzt.
 *
 * @param ptr beliebiger %Pointer
 */
void Pointer::set(const void *ptr)
{
	myptr=(void*)ptr;
}

/*!\brief %Pointer kopieren
 *
 * \desc
 * Mit diesem Operator wird der Wert des %Pointers \p other übernommen.
 *
 * @param other Andere Instanz von %Pointer
 * @return Liefert eine Referenz auf die Klasse selbst zurück
 */
Pointer &Pointer::operator=(const Pointer &other)
{
	myptr=other.myptr;
	return *this;
}

/*!\brief %Pointer zuweisen
 *
 * \desc
 * Ein beliebiger C-Pointer wird der Klasse zugewiesen.
 *
 * @param ptr beliebiger %Pointer
 * @return Liefert eine Referenz auf die Klasse selbst zurück
 */
Pointer &Pointer::operator=(const void *ptr)
{
	myptr=(void*)ptr;
	return *this;
}

bool Pointer::operator<(const Pointer &other) const
{
	if (this->myptr<other.myptr) return true;
	return false;
}

bool Pointer::operator<=(const Pointer &other) const
{
	if (this->myptr<=other.myptr) return true;
	return false;
}

bool Pointer::operator==(const Pointer &other) const
{
	if (this->myptr==other.myptr) return true;
	return false;
}

bool Pointer::operator!=(const Pointer &other) const
{
	if (this->myptr!=other.myptr) return true;
	return false;
}

bool Pointer::operator>=(const Pointer &other) const
{
	if (this->myptr>=other.myptr) return true;
	return false;
}

bool Pointer::operator>(const Pointer &other) const
{
	if (this->myptr>other.myptr) return true;
	return false;
}



} // EOF namespace ppl7
