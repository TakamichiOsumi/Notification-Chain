# Notification-Chain

Mini project to understand the notification chain mechanism.

Notification chain is a common event-driven architecture between two role of entities - *publisher* and *subscriber*.

* *Publisher* is an entity that triggers some specific event.
* *Subscriber* is another type of entity interested in the publisher's event and responds to an occurrence of the event for its work.

In the sample program threads play both roles.

The publisher maintains a routing table that has several rows of IP information (such as destination IP address, mask, etc) as a sample data. It updates the table according to user input from console. During the update process, the publisher calls `rt_add_or_update_rt_entry` that internally invokes `nfc_invoke_notif_chain` if any update or creation of the record happens. In the function, the publisher executes callback function that subscriber delegates to execute, and at the same time it wakes up the waiting subscriber which is interested in the record. This execution of registered callback and process to wake up subscriber is repeatedly performed for all subscribers interested in that record.

From the subscriber point of view, after launched by the publisher, each subscriber registers its interests for the routing table via `rt_table_register_for_notification`. This function connects the routing table record with subscriber's interest as a form of newly created `notif_chain_elem` object. This object is inserted to a linked list each record has and enables the publisher to collaborate with the subscribers, since the data structure stores subscriber thread information as well. After the registration, subscribers wait for any signals to wake up and when they wake up, they do some its work. In the sample program, they just write some thread unique messages to the console.
