/*
 * Copyright (C) 2014-2016 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUSEDITOR_REFACTORING_H
#define SOLARUSEDITOR_REFACTORING_H

#include <QStringList>
#include <functional>

namespace SolarusEditor {

/**
 * \brief Wraps a refactoring action the user wants to perform.
 *
 * TODO allow to specify which files should be saved before doing the refactoring
 */
class Refactoring {

public:

  using RefactoringFunction = std::function<QStringList()>;

  explicit Refactoring(const RefactoringFunction& function);

  QStringList execute() const;

private:

  RefactoringFunction function;   /**< Function that does the refactoring and
                                   * returns the list of paths modified. */

};

}

#endif
