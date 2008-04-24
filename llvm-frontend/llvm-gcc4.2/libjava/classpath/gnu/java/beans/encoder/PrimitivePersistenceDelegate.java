/* PrimitivePersistenceDelegate.java
 -- A PersistenceDelegate for primitive data types.
   Copyright (C) 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package gnu.java.beans.encoder;

import java.beans.Encoder;
import java.beans.Expression;
import java.beans.PersistenceDelegate;

/**
 * A shared PersistenceDelegate implementation for all primitive types.
 * 
 * @author Robert Schuster (robertschuster@fsfe.org)
 */
public class PrimitivePersistenceDelegate extends PersistenceDelegate
{

  protected Expression instantiate(Object oldInstance, Encoder out)
  {
    // The implementation relies on the fact that every primitive
    // wrapper class has a constructor accepting a String argument.
    // By using these constructors creating a primitive instance
    // depends on the String class only.
    return new Expression(oldInstance, oldInstance.getClass(), "new",
                          new Object[] { oldInstance.toString() });
  }

  protected void initialize(Class type, Object oldInstance, Object newInstance, Encoder out)
  {
    // This is a hack to make serializing primitive arrays work correctly.
    // Instead of modifying an existing primitive instance to make it equal
    // with another instance (which is not possible because primitives are
    // immutable) we create a new instance. This is against the specification
    // of the initialize method but make things work fine.
    out.writeExpression(new Expression(oldInstance, oldInstance.getClass(), "new",
                          new Object[] { oldInstance.toString() }));
  }
  
}
