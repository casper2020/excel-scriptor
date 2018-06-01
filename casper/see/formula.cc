/**
 * @file formula.cc Implementation of formula Holder object
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

#include "casper/see/formula.h"
#include "osal/osalite.h"

/**
 * @brief Constructor
 */
casper::see::Formula::Formula ()
{
    /* empty */
}

/**
 * @brief Destructor
 */
casper::see::Formula::~Formula ()
{
    /* empty */
}

void casper::see::Formula::CalculateDependencies (See& a_see)
{
    // For basic formulas the deps are calculated during load parsing
    OSAL_UNUSED_PARAM(a_see);
}

double casper::see::Formula::SumAllTerms (SymbolTable& a_symtab, FILE* a_logfile)
{
    // Dummy on this class
    OSAL_UNUSED_PARAM(a_symtab);
    OSAL_UNUSED_PARAM(a_logfile);
    return 0.0;
}

double casper::see::Formula::SumIfAllTerms (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile)
{
    OSAL_UNUSED_PARAM(a_symtab);
    OSAL_UNUSED_PARAM(a_criterias);
    OSAL_UNUSED_PARAM(a_logfile);
    return 0.0;
}

casper::Term casper::see::Formula::VLOOKUP (const Term& a_value, const char* const a_lookup_col, const Term& a_result_index, bool a_range_lookup)
{
    OSAL_UNUSED_PARAM(a_value);
    OSAL_UNUSED_PARAM(a_lookup_col);
    OSAL_UNUSED_PARAM(a_result_index);
    OSAL_UNUSED_PARAM(a_range_lookup);
    return casper::Term(casper::Term::EUndefined);
}
