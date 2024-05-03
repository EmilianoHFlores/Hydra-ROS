/* -----------------------------------------------------------------------------
 * Copyright 2022 Massachusetts Institute of Technology.
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Research was sponsored by the United States Air Force Research Laboratory and
 * the United States Air Force Artificial Intelligence Accelerator and was
 * accomplished under Cooperative Agreement Number FA8750-19-2-1000. The views
 * and conclusions contained in this document are those of the authors and should
 * not be interpreted as representing the official policies, either expressed or
 * implied, of the United States Air Force or the U.S. Government. The U.S.
 * Government is authorized to reproduce and distribute reprints for Government
 * purposes notwithstanding any copyright notation herein.
 * -------------------------------------------------------------------------- */
#pragma once
#include <hydra/frontend/mesh_segmenter.h>
#include <kimera_pgmo/MeshDelta.h>
#include <ros/ros.h>
#include <visualization_msgs/Marker.h>

#include "hydra_ros/utils/semantic_ros_publishers.h"

namespace hydra {

class ObjectVisualizer : public MeshSegmenter::Sink {
 public:
  using ObjectCloudPub = SemanticRosPublishers<uint32_t, visualization_msgs::Marker>;

  struct Config {
    std::string module_ns = "~objects";
    bool enable_active_mesh_pub = true;
    bool enable_segmented_mesh_pub = true;
    double point_scale = 0.1;
    double point_alpha = 0.7;
    bool use_spheres = false;
  } const config;

  explicit ObjectVisualizer(const Config& config);

  ~ObjectVisualizer();

  std::string printInfo() const override;

  void call(uint64_t timestamp_ns,
            const kimera_pgmo::MeshDelta& delta,
            const std::vector<size_t>& active,
            const LabelIndices& label_indices) const override;

 protected:
  void publishActiveVertices(uint64_t timestamp_ns,
                             const kimera_pgmo::MeshDelta& delta,
                             const std::vector<size_t>& active) const;

  void publishObjectClouds(uint64_t timestamp_ns,
                           const kimera_pgmo::MeshDelta& delta,
                           const LabelIndices& label_indices) const;

  void fillMarkerFromCloud(const kimera_pgmo::MeshDelta& delta,
                           const std::vector<size_t>& indices,
                           visualization_msgs::Marker& marker) const;

 protected:
  ros::NodeHandle nh_;
  ros::Publisher active_vertices_pub_;
  std::unique_ptr<ObjectCloudPub> segmented_vertices_pub_;

 private:
  inline static const auto registration_ =
      config::RegistrationWithConfig<MeshSegmenter::Sink, ObjectVisualizer, Config>(
          "ObjectVisualizer");
};

void declare_config(ObjectVisualizer::Config& conf);

}  // namespace hydra