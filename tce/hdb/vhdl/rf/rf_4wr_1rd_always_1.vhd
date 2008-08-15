-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.
---------------------------------------------------------------------------------
-- Title        : File for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
--
-- VHDL Architecture RF_lib.rf_4wr_1rd_always_1.symbol
--
-- Created:  10:55:33 11/22/05
--          by - tpitkane.tpitkane (elros)
--          at - 10:55:33 11/22/05
--
-- Generated by Mentor Graphics' HDL Designer(TM) 2004.1 (Build 41)
---------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 11/22/05      1.0     tpitkane   Created
-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
LIBRARY work;
USE work.util.all;

ENTITY rf_4wr_1rd_always_1 IS
   GENERIC( 
      dataw   : integer := 32;
      rf_size : integer := 4
   );
   PORT( 
      clk      : IN     std_logic;
      glock    : IN     std_logic;
      r1load   : IN     std_logic;
      r1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      rstx     : IN     std_logic;
      t1data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t1load   : IN     std_logic;
      t1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      t2data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t2load   : IN     std_logic;
      t2opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      t3data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t3load   : IN     std_logic;
      t3opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      t4data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t4load   : IN     std_logic;
      t4opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r1data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 )
   );

-- Declarations

END rf_4wr_1rd_always_1 ;
---------------------------------------------------------------------------------
-- Title        : File for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
--
-- VHDL Architecture RF_lib.rf_4wr_1rd_always_1.rtl
--
-- Created:  10:55:33 11/22/05
--          by - tpitkane.tpitkane (elros)
--          at - 10:55:33 11/22/05
--
-- Generated by Mentor Graphics' HDL Designer(TM) 2004.1 (Build 41)
---------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 11/22/05      1.0     tpitkane   Created
---------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
LIBRARY work;
USE work.util.all;
ARCHITECTURE rtl OF rf_4wr_1rd_always_1 IS

   -- Architecture declarations
   type   reg_type is array (natural range <>) of std_logic_vector(dataw-1 downto 0 );
   subtype rf_index is integer range 0 to rf_size-1;
   signal reg    : reg_type (rf_size-1 downto 0);

BEGIN

   -----------------------------------------------------------------
   Input : PROCESS (clk, rstx)
   -----------------------------------------------------------------

   -- Process declarations
   variable opc : integer;


   BEGIN
      -- Asynchronous Reset
      IF (rstx = '0') THEN
         -- Reset Actions
         for idx in (reg'length-1) downto 0 loop
             reg(idx) <= (others => '0');
         end loop;  -- idx

      ELSIF (clk'EVENT AND clk = '1') THEN
         IF glock = '0' THEN
            IF t1load = '1' THEN
               opc := conv_integer(unsigned(t1opcode));
               reg(opc) <= t1data;
            END IF;
            IF t2load = '1' THEN
               opc := conv_integer(unsigned(t2opcode));
               reg(opc) <= t2data;
            END IF;
            IF t3load = '1' THEN
               opc := conv_integer(unsigned(t3opcode));
               reg(opc) <= t3data;
            END IF;
            IF t4load = '1' THEN
               opc := conv_integer(unsigned(t4opcode));
               reg(opc) <= t4data;
            END IF;
         END IF;
      END IF;
   END PROCESS Input;

   -----------------------------------------------------------------
   --output : PROCESS (glock, r1load, r1opcode, reg, rstx)
   -----------------------------------------------------------------

   r1data <= reg(conv_integer(unsigned(r1opcode)));

END rtl;
