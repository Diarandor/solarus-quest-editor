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
#include "gui/console.h"
#include "quest_runner.h"
#include <QRegularExpression>

namespace {

/**
 * @brief Wraps a line of plain text in html color tags.
 * @param line A plain text line.
 * @param color The color to set, with "#rrggbb" syntax.
 * @return The HTML colorized line.
 */
QString colorize(const QString& line, const QString& color) {

  return QString("<span style=\"color: %1\">%2</span>").arg(color, line.toHtmlEscaped());
}

QRegularExpression output_regexp("^\\[Solarus\\] \\[(\\d+)\\] (\\w+): (.+)$");
QRegularExpression output_simplify_console_error_regexp("In Lua command: \\[string \".*\"\\]:\\d+: ");

}

/**
 * @brief Creates a console view.
 * @param parent Parent object or nullptr.
 */
Console::Console(QWidget* parent) :
  QWidget(parent) {

  ui.setupUi(this);
}

/**
 * @brief Connects this console to a quest runner.
 * @param quest_runner The quest runner.
 */
void Console::set_quest_runner(QuestRunner& quest_runner) {

  this->quest_runner = &quest_runner;

  connect(ui.command_field, SIGNAL(returnPressed()),
          this, SLOT(command_field_activated()));

  connect(&quest_runner, SIGNAL(running()),
          this, SLOT(quest_running()));
  connect(&quest_runner, SIGNAL(finished()),
          this, SLOT(quest_finished()));
  connect(&quest_runner, SIGNAL(output_produced(QStringList)),
          this, SLOT(quest_output_produced(QStringList)));

}

/**
 * @brief Slot called when the quest execution begins.
 */
void Console::quest_running() {

}

/**
 * @brief Slot called when the quest execution is finished.
 */
void Console::quest_finished() {

}

/**
 * @brief Slot called when the quest execution produced some output lines.
 * @param lines The lines read from the standard output of the quest.
 */
void Console::quest_output_produced(const QStringList& lines) {

  for (const QString& line : lines) {

    QString line_html = decorate_output(line);
    if (line_html.isEmpty()) {
      continue;
    }

    ui.log_view->appendHtml(line_html);
  }
}

/**
 * @brief Slot called when the user wants to execute a Lua instruction from the console.
 */
void Console::command_field_activated() {

  if (quest_runner == nullptr) {
    return;
  }
  if (!quest_runner->is_running()) {
    return;
  }

  const QString& command = ui.command_field->text();

  quest_runner->execute_command(command);
  ui.command_field->command_executed(command);

  // Show the command in the log view.
  // TODO: show it only when receiving its results, to make sure it is displayed
  // just before its results.
  ui.log_view->appendPlainText("> " + command);
}

/**
 * @brief Parses a Solarus output line and returns a decorated version of it.
 *
 * Colors may be added, and parts of the line may be removed or changed.
 *
 * @param line The raw output line.
 * @return The HTML decorated line, or an empty string if this line should not be
 * displayed in the console.
 */
QString Console::decorate_output(const QString& line) {

  if (line.isEmpty()) {
    return line;
  }

  QRegularExpressionMatch match_result = output_regexp.match(line);
  if (!match_result.hasMatch()) {
    // Not a line from Solarus, probably one from the quest.
    return line;
  }

  // 4 captures expected: full line, time, log level, message.
  QString log_level;
  QString message;
  QStringList captures = match_result.capturedTexts();
  if (captures.size() != 4) {
    return line;
  }

  log_level = captures[2];
  message = captures[3];

  // Filter out technical delimiters of commands output.
  if (message.startsWith("====== Begin Lua command #")
      || message.startsWith("====== End Lua command #")) {
    return QString();
  }

  if (log_level == "Error") {
    // Clean specific error messages.
    message.remove(output_simplify_console_error_regexp);
  }

  QString decorated_line = log_level + ": " + message;

  // Colorize warnings and errors.
  if (log_level == "Debug") {
    decorated_line = colorize(decorated_line, "#808080");
  }
  else if (log_level == "Info") {
    decorated_line = colorize(decorated_line, "#0000ff");
  }
  else if (log_level == "Warning") {
    decorated_line = colorize(decorated_line, "#b05000");
  }
  else if (log_level == "Error") {
    decorated_line = colorize(decorated_line, "#ff0000");
  }
  else if (log_level == "Fatal") {
    decorated_line = colorize(decorated_line, "#ff0000");
  }
  else {
    // Unknown log level.
    decorated_line = message;
  }

  return decorated_line;
}
