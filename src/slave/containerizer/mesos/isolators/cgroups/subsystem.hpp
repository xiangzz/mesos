// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __CGROUPS_ISOLATOR_SUBSYSTEM_HPP__
#define __CGROUPS_ISOLATOR_SUBSYSTEM_HPP__

#include <string>

#include <mesos/resources.hpp>

#include <mesos/slave/isolator.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>
#include <process/process.hpp>

#include <stout/nothing.hpp>
#include <stout/try.hpp>

#include "slave/flags.hpp"

namespace mesos {
namespace internal {
namespace slave {

/**
 * An abstraction for cgroups subsystem.
 */
class Subsystem : public process::Process<Subsystem>
{
public:
  /**
   * Attempts to create a specific `Subsystem` object that will
   * contain specific information associated with container.
   *
   * @param flags `Flags` used to launch the agent.
   * @param name The name of cgroups subsystem.
   * @param hierarchy The hierarchy path of cgroups subsystem.
   * @return A specific `Subsystem` object or an error if `create` fails.
   */
  static Try<process::Owned<Subsystem>> create(
      const Flags& flags,
      const std::string& name,
      const std::string& hierarchy);

  virtual ~Subsystem() {}

  /**
   * The cgroups subsystem name of this `Subsystem` object.
   *
   * @return The cgroups subsystem name.
   */
  virtual std::string name() const = 0;

  /**
   * Recover the cgroups subsystem for the associated container.
   *
   * @param containerId The target containerId.
   * @return Nothing or an error if `recover` fails.
   */
  virtual process::Future<Nothing> recover(const ContainerID& containerId);

  /**
   * Prepare the cgroups subsystem for the associated container.
   *
   * @param containerId The target containerId.
   * @return Nothing or an error if `prepare` fails.
   */
  virtual process::Future<Nothing> prepare(const ContainerID& containerId);

  /**
   * Isolate the associated container to cgroups subsystem.
   *
   * @param containerId The target containerId.
   * @param pid The process id of container.
   * @return Nothing or an error if `isolate` fails.
   */
  virtual process::Future<Nothing> isolate(
      const ContainerID& containerId,
      pid_t pid);

  /**
   * Watch the container and report if any resource constraint impacts it.
   *
   * @param containerId The target containerId.
   * @return The resource limitation that impacts the container or an
   *     error if `watch` fails.
   */
  virtual process::Future<mesos::slave::ContainerLimitation> watch(
      const ContainerID& containerId);

  /**
   * Update resources allocated to the associated container in this
   * cgroups subsystem.
   *
   * @param containerId The target containerId.
   * @param resources The resources need to update.
   * @return Nothing or an error if `update` fails.
   */
  virtual process::Future<Nothing> update(
      const ContainerID& containerId,
      const Resources& resources);

  /**
   * Gather resource usage statistics of the cgroups subsystem for the
   * associated container.
   *
   * @param containerId The target containerId.
   * @return The resource usage statistics or an error if gather statistics
   *     fails.
   */
  virtual process::Future<ResourceStatistics> usage(
      const ContainerID& containerId);

  /**
   * Get the run-time status of cgroups subsystem specific properties
   * associated with the container.
   *
   * @param containerId The target containerId.
   * @return The container status or an error if get fails.
   */
  virtual process::Future<ContainerStatus> status(
      const ContainerID& containerId);

  /**
   * Clean up the cgroups subsystem for the associated container. It
   * will be called when destruction to ensure everyting be cleanup.
   * Similar to the isolator `cleanup`, it's likely that the `cleanup`
   * for the subsystem is called for unknown containers (see
   * MESOS-6059). We should ignore the cleanup request if the
   * container is unknown to the subsystem.
   *
   * @param containerId The target containerId.
   * @return Nothing or an error if `cleanup` fails.
   */
  virtual process::Future<Nothing> cleanup(const ContainerID& containerId);

protected:
  Subsystem(const Flags& _flags, const std::string& _hierarchy);

  /**
   * `Flags` used to launch the agent.
   */
  const Flags flags;

  /**
   * The hierarchy path of cgroups subsystem.
   */
  const std::string hierarchy;
};

} // namespace slave {
} // namespace internal {
} // namespace mesos {

#endif // __CGROUPS_ISOLATOR_SUBSYSTEM_HPP__
