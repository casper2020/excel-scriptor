/**
 * @file scanner.cc
 *
 * Copyright (c) 2010-2016 Neto Ranito & Seabra LDA. All rights reserved.
 *
 * This file is part of casper.
 *
 * casper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "casper/scanner.h"

#include "osal/osalite.h"

#include <stdlib.h>

const uint8_t casper::Scanner::k_max_fractional_digits_ = 18;


/**
 * @brief Default constructor.
 */
casper::Scanner::Scanner ()
    : p_(NULL), pe_(NULL), eof_(NULL)
{
    fractional_digits_cnt_ = 0;
}

/**
 * @brief Destructor.
 */
casper::Scanner::~Scanner ()
{
    /* Sanitary base destructor */
}

/**
 * @brief Set input.
 *
 * @param a_expression
 * @param a_length
 */
void casper::Scanner::SetInput (const char* a_expression, size_t a_lenght)
{
    input_ = a_expression;
    p_     = a_expression;
    pe_    = a_expression + a_lenght;
    eof_   = pe_;    
}
