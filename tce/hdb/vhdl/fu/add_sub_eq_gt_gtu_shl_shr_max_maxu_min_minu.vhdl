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
-------------------------------------------------------------------------------
-- Title      : ALU unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu.vhdl
-- Author     : Teemu Pitkänen <teemu.pitkanen@tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2008/01/31
-- Platform   : 
-------------------------------------------------------------------------------
-- OPCODES
--      "000" add
--      "001" sub
--      "010" eq
--      "011" gt
--      "100" gtu
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new_revision
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- Entity declaration for add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu unit's user-defined architecture 
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for shl_shr unit's user-defined architecture 
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

package opcodes_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu is

  constant ADD_OPC  : std_logic_vector(3 downto 0)  := "0000";
  constant SUB_OPC  : std_logic_vector(3 downto 0)  := "0001";
  constant EQ_OPC   : std_logic_vector(3 downto 0)  := "0010";
  constant GT_OPC   : std_logic_vector(3 downto 0)  := "0011";
  constant GTU_OPC  : std_logic_vector(3 downto 0)  := "0100";
  constant SHL_OPC  : std_logic_vector(3 downto 0)  := "0101";
  constant SHR_OPC  : std_logic_vector(3 downto 0)  := "0110";
  constant MAX_OPC  : std_logic_vector(3 downto 0)  := "0111";
  constant MAXU_OPC : std_logic_vector(3 downto 0)  := "1000";
  constant MIN_OPC  : std_logic_vector(3 downto 0)  := "1001";
  constant MINU_OPC : std_logic_vector(3 downto 0)  := "1010";    
end opcodes_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;
use work.opcodes_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu.all;

package shl_shr_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc : std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector;
end shl_shr_pkg;

package body shl_shr_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc: std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector is
    
    constant max_shift : integer := shiftw;        
    variable shift_in : std_logic;
    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    variable y_temp : std_logic_vector_array (0 to max_shift);
    variable y : std_logic_vector(dataw-1 downto 0);
    variable shift_ammount : std_logic_vector(shiftw-1 downto 0);
  begin
    shift_ammount := shft_amount(shiftw-1 downto 0);
    
    if opc = SHR_OPC then
      y_temp(0) := flip_bits(input);
      shift_in := y_temp(0)(0);      
    else
      y_temp(0) := input;
      shift_in := '0';
    end if;
    
    for i in 0 to max_shift-1 loop
      if (shift_ammount(i) = '1') then
        y_temp(i+1)                       := (others => shift_in);
        y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i

    if opc = SHR_OPC then
      y := flip_bits(y_temp(max_shift));
    else
      y :=  y_temp(max_shift);    
    end if;
    return y;
  end shift_func;
end shl_shr_pkg;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.opcodes_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu.all;
use work.shl_shr_pkg.all;
entity add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith is
  generic (
    dataw  : integer := 32;
    --busw   : integer := 32;
    shiftw : integer := 5);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    OPC : in std_logic_vector(3 downto 0);
    R   : out std_logic_vector(dataw-1 downto 0));
end add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith is

begin

  process (A,B,OPC)
  begin  -- process
    case OPC is
      when ADD_OPC =>
         R  <= conv_std_logic_vector(signed(A) + signed(B), R'length);
      when SUB_OPC => 
         R  <= conv_std_logic_vector(signed(A) - signed(B), R'length);
      when EQ_OPC  =>
        if A = B then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;        
      when GT_OPC =>
        if signed(A) > signed(B) then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;        
      when GTU_OPC =>
        -- operation GTU;
        if unsigned(A) > unsigned(B) then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;
      when SHL_OPC =>
        R <= shift_func(B,A,opc,dataw,shiftw);
      when SHR_OPC =>
        R <= shift_func(B,A,opc,dataw,shiftw);
      when MAX_OPC=>
        if (signed(a) >= signed(b)) then
          R <= a;
        else
          R <= b;
        end if;
      when MAXU_OPC =>
        if (unsigned(a) >= unsigned(b)) then
          R <= a;
        else
          R <= b;
        end if;
      when MIN_OPC =>
        if (signed(a) <= signed(b)) then
          R <= a;
        else
          R <= b;
        end if;
      when others =>
        if (unsigned(a) <= unsigned(b)) then
          R <= a;
        else
          R <= b;
        end if;
        
    end case;
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32;
    shiftw : integer := 5);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    t1opcode : in std_logic_vector(3 downto 0);
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_1;

architecture rtl of fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_1 is
  
  component add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(3 downto 0);
      R   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal opc_reg : std_logic_vector(3 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      B   => o1reg,
      OPC => opc_reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous Ret (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');
      opc_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            o1temp  <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            opc_reg <= t1opcode;            
            t1reg   <= t1data;
            o1reg   <= o1temp;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  process (r1)
  begin  -- process
    if busw < dataw then
      if busw > 1 then
        r1data(busw-1) <= r1(dataw-1);
        r1data(busw-2 downto 0) <= r1(busw-2 downto 0);
      else
        r1data(0) <= r1(0);
      end if;
    else
      r1data <= sxt(r1,r1data'length);
    end if;
  end process;
  --r1data <= r1;

end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu latency 2
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32;
    shiftw : integer := 5);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in std_logic_vector(3 downto 0);
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_2;

architecture rtl of fu_add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_always_2 is
  
  component add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(3 downto 0);
      R   : out std_logic_vector(busw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(busw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal opc_reg : std_logic_vector(3 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : add_sub_eq_gt_gtu_shl_shr_max_maxu_min_minu_arith
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      OPC => opc_reg,
      B   => o1reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous Ret (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      opc_reg <= (others => '0');
      
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
            opc_reg <= t1opcode;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          if busw < dataw then
            if busw > 1 then
              r1reg(busw-1) <= r1(dataw-1);
              r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
            else
              r1reg(0) <= r1(0);
            end if;
          else
            r1reg <= sxt(r1,r1data'length);
          end if;          
          --if busw < dataw then
          --  r1reg(busw-1) <= r1(dataw-1);
          --  r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          --else
          --  r1reg <= sxt(r1,busw);
          --end if;
          --r1reg <= r1;          
        end if;

      end if;
    end if;
  end process regs;
  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
