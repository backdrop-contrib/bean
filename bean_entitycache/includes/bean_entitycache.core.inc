<?php

class BeanEntityCacheAPIController extends BeanEntityController {

  public function resetCache(array $ids = NULL) {
    EntityCacheControllerHelper::resetEntityCache($this, $ids);
    parent::resetCache($ids);
  }

}
