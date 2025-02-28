<?php

/**
 * @file
 * Bean entity controller class.
 */

/**
 *
 */
class BeanEntityController extends EntityPlusController {
  /**
 *
 */
  protected function setPlugin(Bean $bean) {
    static $plugins = array();

    if (empty($plugins[$bean->type])) {
      $plugins[$bean->type] = bean_load_plugin_class($bean->type);
      $bean->loadUp($plugins[$bean->type]);
    }
  }

  /**
 *
 */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    foreach ($queried_entities as $entity) {
      $this->setPlugin($entity);
    }
    parent::attachLoad($queried_entities, $revision_id);
  }

  /**
 *
 */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $beans = $ids ? $this->load($ids) : array();

    $deltas = array();
    foreach ($beans as $bean) {
      if ($bean->internalIdentifier()) {
        field_attach_delete('bean', $bean);

        $deltas[] = $bean->identifier();
      }
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Implements EntityPlusController::resetCache().
   */
  public function resetCache(array $ids = NULL) {
    bean_reset(TRUE);
    parent::resetCache($ids);
  }

  /**
   * Implements EntityPlusController::saveRevision().
   */
  protected function saveRevision($entity) {
    global $user;

    $org_uid = $entity->uid;
    $entity->uid = $user->uid;
    $return = parent::saveRevision($entity);
    $entity->uid = $org_uid;

    return $return;
  }
}
