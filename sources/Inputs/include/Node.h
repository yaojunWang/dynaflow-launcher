//
// Copyright (c) 2020, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
//

/**
 * @file  Node.h
 *
 * @brief Node structure header file
 *
 */

#pragma once

#include "Behaviours.h"

#include <memory>
#include <string>
#include <vector>

namespace dfl {
/// @brief Namespace for inputs of Dynaflow launcher
namespace inputs {

class Node;

/**
 * @brief topological voltage level structure
 *
 * aggregate of nodes
 */
struct VoltageLevel {
  using VoltageLevelId = std::string;  ///< Voltage level id

  /**
   * @brief Constructor
   *
   * @param vlid voltage level id
   */
  explicit VoltageLevel(const VoltageLevelId& vlid);

  const VoltageLevelId id;                   ///< id
  std::vector<std::shared_ptr<Node>> nodes;  ///< nodes contained in the voltage level
};

/**
 * @brief topological node structure
 *
 * This implement a graph node concept. It contains only the information required to perform the algorithms and not all information extractable from network file
 */
class Node {
 public:
  using NodeId = std::string;  ///< node id definition

  /**
   * @brief Builder for node
   *
   * This builder will perform connections for voltage level element
   *
   * @param id the node id
   * @param vl the voltage level element containing the node
   * @param nominalVoltage the nominal voltage of the node
   *
   * @returns the built node
   */
  static std::shared_ptr<Node> build(const NodeId& id, const std::shared_ptr<VoltageLevel>& vl, double nominalVoltage);

  const NodeId id;                                      ///< node id
  const std::weak_ptr<VoltageLevel> voltageLevel;       ///< voltage level containing the node
  const double nominalVoltage;                          ///< Nominal voltage of the node
  std::vector<std::shared_ptr<Node>> neighbours;        ///< list of neighbours
  std::vector<Load> loads;                              ///< list of loads associated to this node
  std::vector<Generator> generators;                    ///< list of generators associated to this node
  std::vector<ConverterInterface> converterInterfaces;  ///< list of converter associated to this node

 private:
  /**
   * @brief Constructor
   *
   * Private constructor because we are supposed to use only the factory builder
   *
   * @param id the node id
   * @param vl the voltage level element containing the node
   * @param nominalVoltage the voltage level associated with the node
   */
  Node(const NodeId& id, const std::shared_ptr<VoltageLevel> vl, double nominalVoltage);
};

/**
 * @brief Determines if two nodes are equal
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator==(const Node& lhs, const Node& rhs);

/**
 * @brief Determines if two nodes are different
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator!=(const Node& lhs, const Node& rhs);

/**
 * @brief Determines if a node is inferior to another node
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator<(const Node& lhs, const Node& rhs);

/**
 * @brief Determines if a node is inferior or equal to another node
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator<=(const Node& lhs, const Node& rhs);

/**
 * @brief Determines if a node is superior to another node
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator>(const Node& lhs, const Node& rhs);

/**
 * @brief Determines if a node is superior or equal to another node
 *
 * Used criteria is node id
 *
 * @param lhs first node
 * @param rhs second node
 *
 * @returns status of the comparaison
 */
bool operator>=(const Node& lhs, const Node& rhs);

}  // namespace inputs
}  // namespace dfl
