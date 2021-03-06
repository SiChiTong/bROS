//
// Created by shivesh on 20/8/18.
//

#include "costmap/costmap_publisher.h"

namespace costmap {
CostmapPublisher::CostmapPublisher(std::string topic) :
    Node("costmap_publisher"),
    topic_(topic) {
  publisher_ = this->create_publisher<nav_msgs::msg::OccupancyGrid>(topic_);
  costmap_translation_table_ = new char[256];
  costmap_translation_table_[0] = 0;  // NO obstacle
  costmap_translation_table_[253] = 99;  // INSCRIBED obstacle
  costmap_translation_table_[254] = 100;  // LETHAL obstacle
  costmap_translation_table_[255] = -1;  // UNKNOWN

  for (int i = 1; i < 253; i++) {
    costmap_translation_table_[i] = char(1 + (97 * (i - 1)) / 251);
  }
}

CostmapPublisher::~CostmapPublisher() {
  delete costmap_translation_table_;
}

void CostmapPublisher::publish() {
  publisher_->publish(map_);
}

void CostmapPublisher::prepareMap(Costmap *costmap) {
  unsigned int width = costmap->maxx_ - costmap->minx_;
  unsigned int height = costmap->maxy_ - costmap->miny_;

  nav_msgs::msg::OccupancyGrid map;
  map.header.frame_id = costmap->global_frame_;
  map.header.stamp = ros_clock_.now();

  map.info.resolution = costmap->resolution_;
  map.info.width = width;
  map.info.height = height;
  map.info.origin.position.x = costmap->map_origin_.position.x;
  map.info.origin.position.y = costmap->map_origin_.position.y;
  map.info.origin.position.z = 0.0;
  map.info.origin.orientation.w = 1.0;

  int k = 0;
  map.data.resize(map.info.width * map.info.height);
  for (unsigned int i = costmap->miny_; i < costmap->maxy_; ++i) {
    for (unsigned int j = costmap->minx_; j < costmap->maxx_; ++j) {
      map.data[k++] = costmap_translation_table_[(costmap->map_cell[i * costmap->size_x_ + j]).cost];
    }
  }
  map_.data.resize(map.info.width * map.info.height);
  map_ = map;
}
}
